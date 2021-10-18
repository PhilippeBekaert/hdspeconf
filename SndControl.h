/*! \file SndControl.h
 * \brief ALSA sound card control element C++ wrappers.
 * \author Philippe Bekaert <Philippe.Bekaert@uhasselt.be>
 * \date 20210810,0902,03,04,06,10,12,28
 *
 * See \ref controlplusplus for more details.
 */
/*!
 * \page controlplusplus SndControl and co: ALSA control element RAII style C++ wrappers.
 *
 * ALSA control and hcontrol RAII style C++ wrappers, providing a highly programmer
 * friendly interface to ALSA sound card control elements, and control
 * element value caching.
 *
 * Classes
 * SndBoolControl, SndEnumControl, SndIntControl, SndInt64Control,
 * SndBytesControl and SndIec958Control represent boolean, enumerated,
 * integer, integer64, bytex and IEC958 control elements respectively.
 *
 * Common abstract base class SndControl provides type-independent
 * functionality. The templated SndAnyControl common base class adds 
 * value cache and driver read/write depending on type.
 *
 * Construction
 * ------------
 * 
 * Assume your sound card with ALSA index 2 has
 * a enumerated control element named "Clock Mode" with index 0 on the
 * SND_CTL_ELEM_IFACE_CARD interface. Create a
 * SndEnumControl object for it as follows:
 * 
 *     #include <SndCard.h>
 *     #include <SndControl.h>
 *     ...
 *     SndCard* card = new SndCard(2);
 *     ...
 *     SndEnumControl clockMode = new SndEnumControl(card, "Clock Mode", SndControl::CARD, 0);
 *
 * RAII style:
 *
 *     class MyCard {
 *       ...
 *       SndEnumControl clockMode;
 *       ...
 *       MyCard(unsigned index) 
 *         : SndCard(index)
 *         , clockMode(this, "Clock Mode", SndControl::CARD, 0)
 *       {
 *         ...
 *       }
 *       ...
 *     }
 *     ...
 *     MyCard card(2);
 * 
 * Or, if one does not know (or care to know) the control element value type
 * in advance:
 * 
 *     SndControl* clockMode = SndControl::Create(card, "Clock Mode", SndControl::CARD, 0);
 * 
 * Similar Create() functions exist for creating SndControl objects from
 * a ASCII control element identifier name as well as from snd_ctl_elem_id_t*
 * or snd_hctl_elem_t* handles.
 * 
 * After checking the type (SndControl::getType(), which will be
 * enumerated type in this case), clockMode can be dynamic_cast to a
 * SndEnumControl* in order to access element values:
 * 
 *     dynamic_cast<SndEnumControl*> (clockMode)
 * 
 * SndIntControl, SndInt64Control, SndBoolControl, SndBytesControl
 * and SndIec958Control are similar.
 * 
 * Getting information
 * -------------------
 * 
 * - SndControl::getHandle() returns the snd_hctl_elem_t* handle.
 * - SndControl::getCard() returns a pointer to the SndCard to which the control belongs.
 * - SndControl::getInfo() returns the snd_ctl_elem_info_t info for the control.
 * - SndControl::getId() fills in a copy of the snd_ctl_elem_id_t ID of the element.
 * - SndControl::getAsciiId() returns the ASCII control element identifier name.
 * - SndControl::getName(), SndControl::getInterface(), SndControl::getIndex(), SndControl::getDevice(), SndControl::getSubDevice(),
 * SndControl::getType() and SndControl::getCount() are shortcuts returning name, interface, index,
 * device, subdevice, type and channel count of the control element.
 * - SndControl::isReadable(), SndControl::isWritable(), SndControl::isVolatile(), SndControl::isTlvReadable(),
 * SndControl::isTvlWritable(), SndControl::isTlvCommandable(), SndControl::isValid() test exactly what the
 * function name indicates. SndControl::isUser() tests whether or not the element is
 * a user defined control element.
 * 
 * Reading control values
 * ----------------------
 * 
 * - SndAnyControl::get() reads the values from the driver and returns them as a
 * std::vector<T>& where T corresponds with the ALSA control element value
 * type: int, long, long long, unsigned, unsigned char and snd_aes_iec958_t
 * for SndBoolControl, SndIntControl, SndInt64Control, SndEnumControl,
 * SndBytesControl and SndIec958Control respectively.
 * - SndAnyControl::read() reads the values from the driver into the SndXXXXControl object
 * value cache. See below how to access and modify the cached values directly.
 * 
 * SndAnyControl::get() acquires and holds a lock protecting
 * against race conditions with
 * other threads read()ing, write()ing or otherwise manipulating the cached
 * values. read() does not lock and thus needs protection against race
 * conditions. See below for locking mechanisms.
 * 
 * The SndControl classes install a value change callback to receive
 * notifications of element value changes. The value cache of SndControl
 * objects is updated automatically whenever the driver sends such a change
 * notification. Note that not all ALSA control elements send such
 * notifications, in particular volatile control elements may not.
 * Their values still need to be read on demand.
 * 
 * Writing control values
 * ----------------------
 * 
 * - SndAnyControl::set(const T&) sets the first (and often only) channel value and 
 * writes to the driver.
 * - SndAnyControl::set(const std::vector<T>&) sets multiple channel values and writes to
 * the driver.
 * - SndAnyControl::write() writes the cached values to the driver, performing value
 * range checking if appropriate. The cached values can
 * be read/modified/written as explained below.
 * 
 * Both set() members acquire and hold a lock to protect against race
 * conditions with other threads read()ing or otherwise manipulating the
 * cached values, like a change notification thread. write() does not lock
 * and thus needs protection against race conditions.
 * 
 * Note that for non-volatile elements, write() causes the value change
 * callback to be invoked, which immediately read()s back the written value.
 * The value change callback is invoked from the writing thread.
 * 
 * Cached value access
 * -------------------
 * 
 * - SndAnyControl::value() returns a T& suited for reading or 
 * assigning a single value at a time, with channel index range checking.
 * - SndAnyControl::values() returns a T* to the value array.
 * - Use SndAnyControl::operator[] and SndAnyControl::operator= to access resp.
 * assign cached control element values, with channel index range checking.
 * (Beware of ambiguities when using pointers rather than objects!)
 * - Type casting to std::vector<T>&() yields a reference to the vector 
 * of cached values.
 * - Type casting to T yields (a copy of) the first (often the only) channel 
 * value.
 * 
 * Make sure to do your own channel index range checking when accessing the
 * cached values with SndAnyControl::values() or std::vector<T>& casting.
 * Both SndControl::getCount() and std::vector<T>::size() report the number of
 * channels.
 * 
 * Make sure to synchronise multi-threaded access to the cached values with
 * a SndControl::CacheLocker, like this:
 * 
 *     SndIntControl volume(card, "Playback Volume", SND_CTL_ELEM_IFACE_MIXER);
 *     ...
 *     {
 *       SndControl::CacheLocker(volume);
 *       volume.read();
 *       update volume[0] and volume[1] ...
 *       volume.write();
 *     }
 * 
 * Value range checking
 * --------------------
 * 
 * - SndIntControl::getRange() and SndInt64Control::getRange() provide 
 * integer and integer64 value range checking.
 * - SndEnumControl::getEnumCount() returns the size of an enumeration.
 * - SndAnyControl::write() (and thus also SndAnyControl::set()) perform value range checking before writing
 * to the driver, so you do not have to do it yourself if you can live with
 * detecting out-of-range errors at write time rather than assign time.
 * 
 * TLV data
 * --------
 * 
 * - SndControl::readTlv() reads TLV data, if TLV readable.
 * - SndControl::writeTlv() writes TLV data, if TLV writable.
 * - SndControl::commandTlv() executes TLV command, if TLV commandable.
 * - For control elements that provide conversion-to-from-dB TLV data:
 * . SndControl::getdBRange() returns the dB range for elements that offer dB range TLV
 * data.
 * . SndControl::convertTodB() and SndControl::convertFromdB() convert raw volume values to/from
 * gain in 0.01 dB units.
 * 
 * Value/info/TLV change callback
 * ------------------------------
 * 
 * Set a info, value or TLV change callback with 
 * SndControl::callOnInfoChange(), SndControl::callOnValueChange()
 * resp. SndControl::callOnTlvChange(). 
 *
 * Disable a callback by setting it to nullptr.
 * 
 * The control element values are read from the driver right before
 * the value change callback executes, and the lock is held. Keep your
 * change callbacks as short as possible to not unnecessary delay the
 * change notification thread.
 * 
 * Example:
 * 
 *     class MyClass {
 *       ...
 *       SndEnumControl syncSource;
 *       ...
 *       MyClass(const SndCard* card, ...)
 *         : syncSource(card, "Sync Source", SND_CTL_ELEM_IFACE_CARD)
 *         , ...
 *       {
 *         syncSource.callOnValueChange([this] () {onSyncSourceChange();});
 *         ...
 *       }
 *       ...
 *       void onSyncSourceChange(void)
 *       {
 *         switch (syncSource[0]) {
 *           case ...
 *         };
 *       }
 *       ...
 *     };
 * 
 *
 * Each SndCard object has a event handling thread. The callbacks are invoked
 * from the SndCards event handling thread. Callbacks from the same SndCard
 * are thus automatically serialized and will never cause synchronisation
 * issues among each other.
 *
 * Locking
 * -------
 *
 * SndControl provides two kinds of locking:
 * - SndControl::CacheLocker locks the SndControl element value cache as long
 * as the CacheLocker object exists. The lock is taken by the constructor,
 * and released by the destructor. CacheLocker does not prevent other processes
 * to access the same control element. Neither does it prevent other threads
 * in the same process to access the control element through other SndControl 
 * objects for it.
 * - SndControl::ElemLocker takes a system-wide inter-process lock on the
 * control element referred to in a SndControl object. Such lock is provided
 * by the ALSA core in the linux kernel through the snd_ctl_elem_lock() and 
 * snd_ctl_elem_unlock() calls. The lock is taken by the ElemLocker constructor
 * and released by the destructor. The constructor waits until the lock is
 * acquired, retrying snd_ctl_elem_lock() every milisecond as long as
 * another process holds the lock. ElemLocker does not prevent other threads 
 * in the process holding the lock to access the control element. And neither 
 * does it protect the SndControl value cache.
 *
 * In short:
 * - use SndControl::CacheLocker to protect a SndControl value cache within
 * a process.
 * - use SndControl::ElemLocker to protect against other processes 
 * simultaneously accessing the control element referred to in a SndControl
 * object.
 * - access a control element through a single SndControl object for it
 * in a process.
 * Taking both a CacheLocker and ElemLocker then provides complete 
 * intra-process and system-wide inter-process locking on a control element 
 * and its value cache.
 *
 * Both lockers allow recursive locking. The lock is released by the destructor
 * of the outer-most locker.
 *
 * As always, improper use of the locking mechanisms can lead to deadlock
 * and starvation.
 *
 * Printing the values
 * -------------------
 * 
 * Use operator<<(ostream&, const SndControl&) to print the control element
 * values to the given ostream. Again, make sure to protect against possible
 * race conditions.
 */

#pragma once

#include <ostream>
#include <vector>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <atomic>

#include "Snd.h"

//! \brief ALSA control element wrapper base class.
//!
//! Base class of SndAnyControl, SndBoolControl, SndIntControl, SndInt64Control,
//! SndEnumControl, SndBytesControl, SndIec958Control.
//!
//! The SndControl base class
//! lacks a value cache and methods for reading and driver values from/to
//! the driver. Value cache and read() and write() functions are provided
//! by the derived classes, depending on the control element type.
//!
//! See \ref controlplusplus page for more details.
class SndControl {
protected:
  const class SndCard* card { nullptr }; //!< SndCard to which this control belongs.

  snd_hctl_elem_t* elem { nullptr }; //!< ALSA hcontrol handle.
  snd_ctl_elem_info_t* info { nullptr };  //!< ALSA control element info.

  std::string name;                  //!< Control name.
  snd_ctl_elem_type_t type { SND_CTL_ELEM_TYPE_NONE };  //!< ALSA value type.
  unsigned count { 0 };              //!< Number of channels in control.

  //! \brief Operator<<(std::ostream&, const SndControl&) work horse.
  virtual std::ostream& print(std::ostream& s) const =0;

  //! \brief Throw std::runtime_error if channel is out of range.
  virtual void checkChannel(unsigned channel) const;

  //! \brief Return ALSA name of the card to which this control element belongs.
  const std::string getCardName(void) const;

  //! \brief ALSA element event callback for this control element. Calls
  //! user supplied onInfoChange() and/or onValueChange() depending on mask.
  virtual void onElemEvent(unsigned mask) =0;

private:
  //! \brief ALSA hcontrol element callback.
  static int _elem_cb(snd_hctl_elem_t *elem, unsigned int mask);
  
public:
  //! \brief SND_CTL_ELEM_IFACE_XXXXX redefinitions for conciseness.
  enum Interface {
    CARD      = SND_CTL_ELEM_IFACE_CARD,
    HWDEP     = SND_CTL_ELEM_IFACE_HWDEP,
    MIXER     = SND_CTL_ELEM_IFACE_MIXER,
    PCM       = SND_CTL_ELEM_IFACE_PCM,
    RAWMIDI   = SND_CTL_ELEM_IFACE_RAWMIDI,
    TIMER     = SND_CTL_ELEM_IFACE_TIMER,
    SEQUENCER = SND_CTL_ELEM_IFACE_SEQUENCER
  };
  
  //! \brief SndControl factory: creates a SndBoolControl, SndIntControl, ...
  //! depending on the type of the ALSA control element, if found. Throws
  //! std::runtime_error with appropriate message in case
  //! no control element with given name, interface and index
  //! is found on the card. Use dynamic_cast<SndBoolControl*> etc... to
  //! downcast the returned pointer for element access.
  static SndControl* Create(const class SndCard* card,
			    const std::string& name,
			    Interface iface =CARD,
			    unsigned index =0)
  {
    return Create(card, Find(card, name, iface, index));
  }

  //! \brief Same, creating from an ascii control element identifier name.
  static SndControl* CreateFromAsciiId(const class SndCard* card,
				       const std::string& asciiId)
  {
    return Create(card, FindFromAsciiId(card, asciiId));
  }

  //! \brief Same, creating from a snd_hctl_elem_t* handle.
  static SndControl* Create(const class SndCard* card, snd_hctl_elem_t* elem);

  //! \brief Find the control element with given name, interface and index on
  //! the given card. Throws std::runtime_error if not found. Called by
  //! Create().
  static snd_hctl_elem_t* Find(const class SndCard* card,
			       const std::string& name,
			       Interface iface,
			       unsigned index);

  //! \brief Find the control element with given ascii identifier name on the
  //! given card. Throws std::runtime_error if not found. Called by Create().
  static snd_hctl_elem_t* FindFromAsciiId(const class SndCard* card,
					  const std::string& asciiId);
  
  //! \brief Find the control element with given snd_ctl_elem_id_t identifier.
  //! Throws std::runtime_error if not found. Called by the other Find()
  //! functions and Create().
  static snd_hctl_elem_t* Find(const class SndCard* card, snd_ctl_elem_id_t* id);

  //! \brief Constructor: called by Create() or derived classes constructors.
  //! You do not want to use this particular abstract base class constructor.
  //! Use Create() or the SndBoolControl etc... constructors instead.
  SndControl(const class SndCard* card, snd_hctl_elem_t* elem);
  
  //! \brief Desctructor. Releases the element control handle and
  //! de-allocates resources.
  virtual ~SndControl();

  //! \brief Get the card to which this control belongs.
  const class SndCard* getCard(void) const      { return card; }

  //! \brief Get ALSA hcontrol element handle.
  snd_hctl_elem_t* getHandle(void) const        { return elem; }

  //! \brief Get ALSA element info.
  snd_ctl_elem_info_t* getInfo(void) const      { return info; }

  //! \brief Get ALSA control name.
  const std::string& getName(void) const         { return name; }

  //! \brief Get a copy of the snd_ctl_elem_id_t of the control element.
  //! \note You need to allocate storage for a snd_ctl_elem_id_t with
  //! snd_ctl_elem_id_alloca() or consorts before calling this function.
  void getId(snd_ctl_elem_id_t* id) const
  {
    snd_hctl_elem_get_id(elem, id);
  }

  //! \brief Get ASCII control element identifier name.
  const std::string getAsciiId(void) const;
  
  //! \brief Get the ALSA interface to which this control is connected.
  snd_ctl_elem_iface_t getInterface(void) const
  {
    return snd_hctl_elem_get_interface(elem);
  }
  
  //! \brief Get the ALSA index of this control.
  unsigned getIndex(void) const   
  {
    return snd_hctl_elem_get_index(elem);
  }

  //! \brief Get the ALSA device index of this control.
  unsigned getDevice(void) const
  {
    return snd_hctl_elem_get_device(elem);
  }
    
  //! \brief Get the ALSA subdevice of this control.
  unsigned getSubDevice(void) const 
  {
    return snd_hctl_elem_get_subdevice(elem);
  }

  //! \brief Get the ALSA value type of this control.
  snd_ctl_elem_type_t getType(void) const       { return type; }

  //! \brief Get the number of channels in this control.
  int getCount(void) const                      { return count; }

  //! \brief Test whether this control element is a user generated
  //! control element. Returns true if so, and false if it is a driver
  //! control element.
  bool isUser(void) const
  {
    return snd_ctl_elem_info_is_user(info);
  }

  //! \brief Check whether the control is active. Returns true if active
  //! and false if not active.
  bool isActive(void) const
  {
    return snd_ctl_elem_info_is_inactive(info);
  }

  //! \brief Check whether the control is readable.
  bool isReadable(void) const 
  {
    return snd_ctl_elem_info_is_readable(info);
  }

  //! \brief Check whether the control is writable.
  bool isWritable(void) const    
  {
    return snd_ctl_elem_info_is_writable(info);
  }

  //! \brief Check whether the control is volative. That means that the
  //! value of the control may change without notification, and value
  //! caching does not work.
  bool isVolatile(void) const 
  {
    return snd_ctl_elem_info_is_volatile(info);
  }

  //! \brief Check whether the control is TLV readable.
  bool isTlvReadable(void) const 
  {
    return snd_ctl_elem_info_is_tlv_readable(info);
  }

  //! \brief Check whether the control is TLV writable.
  bool isTlvWritable(void) const    
  {
    return snd_ctl_elem_info_is_tlv_writable(info);
  }
  
  //! \brief Check whether the control is TLV commandable.
  bool isTlvCommandable(void) const 
  {
    return snd_ctl_elem_info_is_tlv_commandable(info);
  }

  //! \brief Read TLV value from the driver.
  //!
  //! \param tlv buffer to receive the structured data from the element
  //! set. The format of an array of tlv argument is: tlv[0]: Type. One
  //! of SND_CTL_TLVT_XXX. tlv[1]: Length. The length of value in units
  //! of byte. tlv[2..]: Value. Depending on the type.
  //! Details are described in <sound/tlv.h>.
  //!
  //! \param tlv_size the size of the tlv buffer.
  //!
  //! \return Returns 0 if
  //! succesful, and a negative error code otherwise.
  int readTlv(unsigned int *tlv, unsigned int tlv_size)
  {
    return snd_hctl_elem_tlv_read(elem, tlv, tlv_size);
  }
  
  //! \brief Write TLV value to the driver.
  //!
  //! \param tlv structured data to be written to the element set.
  //! The format of an array of tlv argument is: tlv[0]: Type.
  //! One of SND_CTL_TLVT_XXX. tlv[1]: Length. The length of value in
  //! units of byte. tlv[2..]: Value. Depending on the type.
  //! Details are described in <sound/tlv.h>. 
  //!
  //! \return Returns 1 if the value
  //! was changed, 0 if succesful but value is unchanged, 
  //! and a negative error code otherwise.
  int writeTlv(const unsigned int *tlv)
  {
    return snd_hctl_elem_tlv_write(elem, tlv);
  }

  //! \brief Process structured data from given buffer for an element set.
  //!
  //! \param tlv structured data to be written to the element set.
  //! The format of an array of tlv argument is: tlv[0]: Type.
  //! One of SND_CTL_TLVT_XXX. tlv[1]: Length. The length of value in
  //! units of byte. tlv[2..]: Value. Depending on the type.
  //! Details are described in <sound/tlv.h>. 
  //!
  //! \return Returns 1 if the value
  //! was changed, 0 if succesful but value unchaged, 
  //! and a negative error code otherwise.
  int commandTlv(const unsigned int *tlv)
  {
    return snd_hctl_elem_tlv_command(elem, tlv);
  }
  
  //! \brief Get the dB range in 0.01 dB units of the control element. Returns 0
  //! if succesful, and a negative error code otherwise.
  int getdBRange(long* min, long* max) const;

  //! \brief Convert raw volume value to dB gain, in 0.01 dB units. Returns
  //! 0 if succesful, and a negative error code otherwise.
  int convertTodB(long volume, long* db_gain) const;

  //! \brief Convert dB gain in 0.01dB units, to raw volume value.Returns
  //! 0 if succesful, and a negative error code otherwise. xdir determines
  //! the direction of rounding: up if positive, down if negative.
  int convertFromdB(long db_gain, long* volume, int xdir) const;

  //! \brief Read control element values from the driver, into the value cache.
  virtual void read(void) =0;

  //! \brief Write cached control element values to the driver.
  //! write() throws a std::runtime_error if the element is not writable,
  //! another thread has syslock()'ed the element, or cached values are
  //! out of range.
  virtual void write(void) =0;

  //! \brief Info/Value/TLV change callback function.
  using Callback = std::function<void(void)>;

  //! \brief Set callback to be called upon change of control element
  //! info. Setting nullptr disables.
  //! \param cb : callback function to be invoked upon change of info.
  //! \return Returns the callback that was replaced by the new callback.
  //!
  //! Immediately invokes cb if not nullptr, from the current thread.
  //!
  //! Later invocations are from the SndCard's event handling thread.
  Callback callOnInfoChange(Callback cb)
  {
    Callback old = onInfoChange;
    onInfoChange = cb;
    if (cb) cb();    
    return old;
  }

  //! \brief Set callback to be called upon change of control element
  //! value. Setting nullptr disables.
  //! \param cb : callback function to be invoked upon change of value.
  //! \return Returns the callback that was replaced by the new callback.
  //!
  //! Immediately invokes cb if not nullptr, from the current thread.
  //!
  //! Later invocations are from the SndCard's event handling thread.
  //!
  //! If the control element is readable, its new value is read
  //! right before invoking the callback.
  Callback callOnValueChange(Callback cb)
  {
    Callback old = onValueChange;
    onValueChange = cb;
    if (cb) cb();
    return old;
  }

  //! \brief Set callback to be called upon change of control element
  //! TLV. Setting nullptr disables.
  //! \param cb : callback function to be invoked upon change of TLV.
  //! \return Returns the callback that was replaced by the new callback.
  //!
  //! Immediately invokes cb if not nullptr, from the current thread.
  //!
  //! Later invocations are from the SndCard's event handling thread.
  Callback callOnTlvChange(Callback cb)
  {
    Callback old = onTlvChange;
    onTlvChange = cb;
    if (cb) cb();
    return old;
  }

  //! \brief Print control element c value to the stream s.
  friend std::ostream& operator<<(std::ostream& s, const SndControl& c)
  {
    return c.print(s);
  }

  //! \brief Essentially like std::lock_guard<>, but takes 
  //! a SndControl object or pointer as argument rather than a
  //! mutex, for convenience.
  //! The constructor locks the SndControl:rmtx mutex. The destructor unlocks.
  //!
  //! SndControl::CacheLocker allow recursive locking: SndControl::rmtx is
  //! a recursive mutex.
  //!
  //! SndControl::CacheLocker only protects the cached values against
  //! race conditions (between threads of the running proces). It does not
  //! lock the element system-wide: other processes may read/write
  //! the control element while this thread holds a CacheLocker.
  //! Use a SndControl::ElemLocker to prevent that.
  //!
  //! Avoid having several SndControl objects for the same control
  //! element in one process: there's nothing preventing other process threads
  //! from accessing the control element using a different SndControl object.
  class CacheLocker {
  private:
    SndControl* ctl { nullptr }; //!< SndControl object being locked.

    void lock(void) { ctl->rmtx.lock(); }
    void unlock(void) { ctl->rmtx.unlock(); }
    
  public:
    //! \brief Constructor: locks ctl mutex.
    //! \param ctl : reference to the SndControl object to be protected.
    CacheLocker(SndControl& _ctl) : ctl(&_ctl) { lock(); }

    //! \brief Constructor: locks ctl mutex.
    //! \param ctl : pointer to the SndControl object to be protected.
    CacheLocker(SndControl* _ctl) : ctl(_ctl) { lock(); }

    //! \brief Destructor: unlocks ctl mutex.
    ~CacheLocker() { unlock(); }
  };

  //! \brief Essentially like std::lock_guard<> but takes the ALSA core
  //! provided system-wide inter-process lock on the control element.
  //!
  //! ElemLocker and CacheLocker are complementary:
  //! CacheLocker protects against race conditions between
  //! intra-process threads accessing the cached control element values.
  //! ElemLocker provides inter-process locking of the control element.
  //! 
  //! Just like CacheLockers, ElemLockers allows recursive locking. The
  //! inter-process lock is released when the outer-most ElemLocker is
  //! destroyed.
  //!
  //! ElemLocker only prevents against other *processes* accessing the element.
  //! Other *threads* of this process still can. Avoid different threads in a
  //! process accessing the control element through different SndControl
  //! objects.
  class ElemLocker {
  private:
    std::atomic_uint refCnt {0};
    SndControl* ctl { nullptr };  //!< Refers to the control element object being locked.

    void lock(void)
    {
      while (!ctl->try_lock()) { usleep(1000); }
      refCnt++;
    }

    void unlock(void)
    {
      if (--refCnt == 0)
	ctl->unlock();
    }
    
  public:
    //! \brief Constructor: waits until the inter-process lock on the control
    //! element is obtained.
    //! \param ctl : reference to the SndControl object to be protected.
    ElemLocker(SndControl& _ctl) : ctl(&_ctl) { lock(); }

    //! \brief Same, with SndControl pointer argument rather then reference.
    //! \param ctl : pointer to the SndControl object to be protected.    
    ElemLocker(SndControl* _ctl) : ctl(_ctl) { lock(); }

    //! \brief Destructor: releases the inter-process control element lock.
    ~ElemLocker() { unlock(); }
  };
  
protected:
  Callback onValueChange { nullptr };   //!< Valued change user callback. Set to nullptr to disable.
  Callback onInfoChange { nullptr };    //!< Info change user callback. Set to nullptr to disable.
  Callback onTlvChange { nullptr };     //!< TLV change user callback. Set to nullptr to disable.

  //! \brief Tries to acquire the ALSA core provided system-wide inter-process
  //! lock on this control element. Other processes
  //! trying to write the control elements values will fail with a
  //! EPERM error code when this process owns that lock on it.
  //!
  //! \return Returns true if the element lock has been acquired. Returns false
  //! if some other process already owns the control element lock.
  //!
  //! \note try_lock() does not protect the cached values against race
  //! conditions (between threads of the current proces). Use the
  //! SndControl::rmtx mutex for that purpose.
  bool try_lock(void);

  //! \brief System-wide unlock the control element.
  void unlock(void);

  std::recursive_mutex rmtx;   //!< Synchronises access to the cached values.
};

//! \brief ALSA control element wrapper templated base class, supplementing
//! SndControl with a value cache of type T, and methods for accessing cached
//! values.
//!
//! SndBoolControl, SndIntControl, SndInt64Control, SndEnumControl,
//! SndBytesControl and SndIec958Control are specialisations of this class,
//! with T being int, long, long long, unsigned, unsigned char and
//! snd_aes_iec958_t respectively.
//!
//! See \ref controlplusplus page for more details.
template<typename T>
class SndAnyControl: public SndControl {
 protected:
  std::vector<T> val;    //!< Control element value cache.

  //! \brief throw std::runtime_errror with appropriate message
  //! in case this controls ALSA type is not the required type.
  void checkType(snd_ctl_elem_type_t required_type, const std::string& type_name)
  {
    if (type != required_type)
      throw std::runtime_error("SndControl '" + name + "' on card '"
			       + getCardName() + "' is not a "
			       + type_name + " control element.\n");
  }

  //! \brief Read element values from driver, using the provided
  //! getter() function (like snd_ctl_elem_value_get_int() etc...).
  void read(std::function<T(snd_ctl_elem_value_t* ctl,
			    unsigned channel)> getter)
  {
    if (!isReadable())
      throw std::runtime_error("SndControl '" + name
			       + "' on card '" + getCardName()
			       + "' is not readable.\n");
    
    snd_ctl_elem_value_t *ctl;
    snd_ctl_elem_value_alloca(&ctl);
    SndCheckErr(snd_hctl_elem_read(elem, ctl), "hctl_elem_read");

    CacheLocker g(this);
    val.resize(count);
    for (unsigned i=0; i<count; i++)
      val[i] = getter(ctl, i);
  }

  virtual void read(void) =0;

  //! \brief Write cached element values to driver using the provided
  //! setter() function (like snd_ctl_elem_value_set_int() etc...).
  //! Performs value range checking with valid().
  void write(std::function<void(snd_ctl_elem_value_t* ctl, unsigned channel,
				T value)> setter,
	     std::function<bool(T value)> valid)
  {
    if (!isWritable())
      throw std::runtime_error("SndControl '" + name
			       + "' on card '" + getCardName()
			       + "' is not writable.\n");
    
    snd_ctl_elem_value_t *ctl;
    snd_ctl_elem_value_alloca(&ctl);

    { CacheLocker g(this); 
    for (unsigned i=0; i<count; i++) {
      if (!valid(val[i]))
	throw std::runtime_error("SndControl '" + name
				 + "' on card '" + getCardName()
				 + "' channel " + std::to_string(i)
				 + " value " + to_string(val[i])
				 + " out of range.\n");
      setter(ctl, i, val[i]);
    }}
    
    SndCheckErr(snd_hctl_elem_write(elem, ctl), "hctl_elem_write");  
  }

  virtual void write(void) =0;

  //! \brief Converts a single value to a std::string.
  virtual const std::string to_string(T) const =0;

  virtual void onElemEvent(unsigned mask)
  {
    if ((mask & SND_CTL_EVENT_MASK_INFO) && onInfoChange)
      onInfoChange();
    if ((mask & SND_CTL_EVENT_MASK_TLV) && onTlvChange)
      onTlvChange();
    if ((mask & SND_CTL_EVENT_MASK_VALUE)) {
      if (isReadable())
	read();
      if (onValueChange) {
	CacheLocker g(this);
	onValueChange();
      }
    }
  }
  
 public:
  SndAnyControl(const SndCard* card, snd_hctl_elem_t* elem,
		snd_ctl_elem_type_t required_type, const std::string& type_name)
    : SndControl(card, elem)
  {
    checkType(required_type, type_name);
    val.resize(count);

    snd_hctl_elem_set_callback_private(elem, this);
    snd_hctl_elem_set_callback(elem, SndControl::_elem_cb);
  }

  virtual ~SndAnyControl() {}

  //! \brief Read values from driver and returns a copy to the caller.
  //! The returned copy is guaranteed to be a consistent set.
  const std::vector<T> get(void)
  {
    read();
    CacheLocker g(this);    
    return val;
  }
  
  //! \brief Set the first channels value and write to driver.
  //! \note The value may be changed by another thread in between setting
  //! the value here, and writing it. If this is not desirable, protect set()
  //! by enclosing it in a SndControl::CacheLocker scope.
  void set(const T& value)
  {
    {CacheLocker g(this);        
    checkChannel(0);
    val[0] = value;}
    write();
  }
  
  //! \brief Assign new values to cache and write to driver.
  //!
  //! The number of values assigned is limited by both the number
  //! of channels in the control element (count) and newval.size().
  //!
  //! The values may be changed by another thread in between setting
  //! the values here, and writing them. However, provided the other thread
  //! correctly locks the values when setting and writing them, with a
  //! SndControl::CacheLocker, the values written are either all as set here,
  //! or all as set by the other thread, but not a mixture of both.
  //!
  //! Protect set() by enclosing it in SndControl::CacheLocker scope if
  //! you want to make sure the values written are effectively those set here,
  //! but keep in mind that nothing prevents another thread to write its values
  //! just next after.
  //!
  //! Use SndControl::try_lock() ... SndControl::unlock(n) to prevent other
  //! applications to write control values, if desired.
  void set(const std::vector<T>& newval)
  {
    {CacheLocker g(this);            
      *this = newval;}
    write();
  }

  //! \brief Get reference to the cached value for the indicated channel.
  //! Protect against race conditions.
  T& value(int channel =0)
  {
    checkChannel(channel);
    return val[channel];
  }

  //! \brief Same, read-only access.
  const T& value(int channel =0) const
  {
    checkChannel(channel);
    return val[channel];
  }

  //! \brief Return pointer to the array of cached values.
  //! Protect against race conditions.
  T* values(void)
  {
    return val.data();
  }

  //! \brief Same, read-only access
  const T* const values(void) const
  {
    return val.data();
  }
  
  //! \brief Cached element value access, without read() or write().
  //! Protect against race conditions.
  T& operator[](int channel)
  {
    checkChannel(channel);
    return val[channel];
  }

  //! \brief Same, for read-only access.
  const T& operator[](int channel) const
  {
    checkChannel(channel);
    return val[channel];
  }

  //! \brief Get reference to the std::vector<T> holding the cached values.
  //! Protect against race conditions.
  operator std::vector<T>&()
  {
    return val;
  }

  //! \brief Same, read-only access.
  operator const std::vector<T>&() const
  {
    return val;
  }

  //! \brief Get the first channel value.
  //! Protect against race conditions.
  operator const T() const
  {
    checkChannel(0);
    return val[0];
  }

  //! \brief Assign new values to the cache. The number of values assigned is
  //! limited by both count and newvals.size(). Does not write() to driver.
  //! Protect against race conditions.
  const std::vector<T>& operator=(const std::vector<T>& newval)
  {
    for (size_t i=0; i<val.size() && i<newval.size(); i++) 
      val[i] = newval[i];
    return val;
  }

  //! \brief Assign new value to the first channel. Does not write() to driver.
  //! Protect against race conditions.
  const T& operator=(const T newval)
  {
    checkChannel(0);
    return val[0] = newval;  // performs range check
  }
};

//! \brief Boolean ALSA control element wrapper.
//!
//! See \ref controlplusplus page for more details.
class SndBoolControl: public SndAnyControl<int> {
protected:
  std::ostream& print(std::ostream& s) const override
  {
    for (auto v: val)
      s << v << " ";
    return s;
  }

  const std::string to_string(int v) const override
  {
    return std::to_string(v);
  }

public:
  SndBoolControl(const class SndCard* card,
		 const std::string& name,
		 Interface iface =CARD,
		 unsigned index =0)
    : SndAnyControl(card, Find(card, name, iface, index),
		    SND_CTL_ELEM_TYPE_BOOLEAN, "boolean") { if (isReadable()) read(); }

  SndBoolControl(const class SndCard* card,
		 snd_hctl_elem_t* elem)
    : SndAnyControl(card, elem,
		    SND_CTL_ELEM_TYPE_BOOLEAN, "boolean") { if (isReadable()) read(); }

  void read(void) override
  {
    SndAnyControl::read(snd_ctl_elem_value_get_boolean);
  }

  void write(void) override
  {
    /* snd_ctl_elem_value_get_boolean() returns an int value, but
     * snd_ctl_elem_value_set_boolean() takes a long value. Lambda
     * function here serves to convert. */
    SndAnyControl::write(
      [](snd_ctl_elem_value_t* ctl, unsigned idx, int value){
	snd_ctl_elem_value_set_boolean(ctl, idx, value);
      },
      [](int v){return true;} );
  }
};

//! \brief Integer ALSA control element wrapper.
//!
//! See \ref controlplusplus page for more details.
class SndIntControl: public SndAnyControl<long> {
protected:
  std::ostream& print(std::ostream& s) const override
  {
    for (auto v: val)
      s << v << " ";
    return s;
  }

  const std::string to_string(long v) const override
  {
    return std::to_string(v);
  }

public:
  SndIntControl(const class SndCard* card,
		const std::string& name,
		Interface iface =CARD,
		unsigned index =0)
    : SndAnyControl(card, Find(card, name, iface, index),
		    SND_CTL_ELEM_TYPE_INTEGER, "integer") { if (isReadable()) read(); }

  SndIntControl(const class SndCard* card,
		snd_hctl_elem_t* elem)
    : SndAnyControl(card, elem,  
		    SND_CTL_ELEM_TYPE_INTEGER, "integer") { if (isReadable()) read(); }

  //! \brief Get mininum and maximum of valid value range, as well
  //! as the value step.
  void getRange(long *min, long *max, long *step) const
  {
    *min = snd_ctl_elem_info_get_min(info);
    *max = snd_ctl_elem_info_get_max(info);
    *step = snd_ctl_elem_info_get_step(info);
  }

  void read(void) override
  {
    SndAnyControl::read(snd_ctl_elem_value_get_integer);
  }

  void write(void) override
  {
    long m, M, step;
    getRange(&m, &M, &step);
        
    SndAnyControl::write(snd_ctl_elem_value_set_integer,
			 [m,M,step] (long v) {
			   return ((m==0 && M==0) || (v>=m && v<=M))
			     && (step==0 || (v-m)%step==0);
			 });
  }
};

//! \brief Integer64 ALSA control element wrapper.
//!
//! See \ref controlplusplus page for more details.
class SndInt64Control: public SndAnyControl<long long> {
protected:
  std::ostream& print(std::ostream& s) const override
  {
    for (auto v: val)
      s << v << " ";
    return s;
  }
  
  const std::string to_string(long long v) const override
  {
    return std::to_string(v);
  }

public:
  SndInt64Control(const class SndCard* card,
		 const std::string& name,
		 Interface iface =CARD,
		 unsigned index =0)
    : SndAnyControl(card, Find(card, name, iface, index),
		    SND_CTL_ELEM_TYPE_INTEGER64, "integer64") { if (isReadable()) read(); }

    SndInt64Control(const class SndCard* card,
		    snd_hctl_elem_t* elem)
    : SndAnyControl(card, elem,  
		    SND_CTL_ELEM_TYPE_INTEGER64, "integer64") { if (isReadable()) read(); }
  
  //! \brief Get mininum and maximum of valid value range, as well
  //! as the value step.
  void getRange(long long *min, long long *max, long long *step) const
  {
    *min = snd_ctl_elem_info_get_min64(info);
    *max = snd_ctl_elem_info_get_max64(info);
    *step = snd_ctl_elem_info_get_step64(info);
  }
  
  void read(void) override
  {
    SndAnyControl::read(snd_ctl_elem_value_get_integer64);
  }

  void write(void) override
  {
    long long m, M, step;
    getRange(&m, &M, &step);
        
    SndAnyControl::write(snd_ctl_elem_value_set_integer64,
			 [m,M,step] (long long v) {
			   return ((m==0 && M==0) || (v>=m && v<=M))
			     && (step==0 || (v-m)%step==0);
			 });
  }
};

//! \brief Enumerated ALSA control element wrapper.
//!
//! See \ref controlplusplus page for more details.
class SndEnumControl: public SndAnyControl<unsigned> {
protected:
  std::ostream& print(std::ostream& s) const override
  {
    for (unsigned i=0; i<count; i++)
      s << val[i] << " '" << getEnumLabel(val[i]) << "' ";
    return s;
  }

  const std::string to_string(unsigned v) const override
  {
    return std::to_string(v);
  }
  
public:
  SndEnumControl(const class SndCard* card,
		 const std::string& name,
		 Interface iface =CARD,
		 unsigned index =0)
    : SndAnyControl(card, Find(card, name, iface, index),
		    SND_CTL_ELEM_TYPE_ENUMERATED, "enumerated") { if (isReadable()) read(); }

  SndEnumControl(const class SndCard* card,
		 snd_hctl_elem_t* elem)
    : SndAnyControl(card, elem,  
		    SND_CTL_ELEM_TYPE_ENUMERATED, "enumerated") { if (isReadable()) read(); }  
  
  //! \brief Return the number of enum items.
  unsigned getEnumCount(void) const
  {
    return snd_ctl_elem_info_get_items(info);
  }
  
  //! \brief Return label string for enum value.
  const std::string getEnumLabel(unsigned value) const
  {
    snd_ctl_elem_info_set_item(info, value);
    SndCheckErr(snd_hctl_elem_info(elem, info), "hctl_elem_info");
    return std::string(snd_ctl_elem_info_get_item_name(info));
  }

  //! \brief Return label string for the current value in channel i
  const std::string label(int i =0) const
  {
    checkChannel(i);
    return getEnumLabel(val[i]);
  }

  void read(void) override
  {
    SndAnyControl::read(snd_ctl_elem_value_get_enumerated);
  }

  void write(void) override
  {
    unsigned count = getEnumCount();
    
    SndAnyControl::write(snd_ctl_elem_value_set_enumerated,
			 [count] (unsigned value) { return value<count; }
			 );
  }
};

//! \brief Bytes ALSA control element wrapper.
//!
//! See \ref controlplusplus page for more details.
class SndBytesControl: public SndAnyControl<unsigned char> {
protected:
  //! \brief Print data hexadecimally.
  std::ostream& print(std::ostream& s) const override
  {
    s << std::hex;
    for (auto v: val)
      s << v;
    return s << std::dec;
  }

  const std::string to_string(unsigned char v) const override
  {
    return std::to_string(v);
  }
  
public:
  SndBytesControl(const class SndCard* card,
		  const std::string& name,
		  Interface iface =CARD,
		  unsigned index =0)
    : SndAnyControl(card, Find(card, name, iface, index),
		    SND_CTL_ELEM_TYPE_BYTES, "bytes") { if (isReadable()) read(); }

  SndBytesControl(const class SndCard* card,
		  snd_hctl_elem_t* elem)
    : SndAnyControl(card, elem,  
		    SND_CTL_ELEM_TYPE_BYTES, "bytes") { if (isReadable()) read(); }

  void read(void) override
  {
    SndAnyControl::read(snd_ctl_elem_value_get_byte);
  }

  void write(void) override
  {
    SndAnyControl::write(snd_ctl_elem_value_set_byte,
			 [] (unsigned char v) { return true; });
  }
};

//! \brief IEC958 ALSA control element wrapper.
//!
//! See \ref controlplusplus page for more details.
class SndIec958Control: public SndAnyControl<snd_aes_iec958_t> {
protected:
  //! \brief Print IEC958 data in hex.
  std::ostream& print(std::ostream& s) const override
  {
    if (count < 1)
      return s;
    
    unsigned char* v = (unsigned char*)&val[0];
    s << std::hex;
    for (unsigned i=0; i<(int)sizeof(snd_aes_iec958_t); i++)
      s << *v++;
    return s << std::dec;
  }
  
  const std::string to_string(snd_aes_iec958_t v) const override
  {
    return "...";
  }

public:
  SndIec958Control(const class SndCard* card,
		 const std::string& name,
		 Interface iface =CARD,
		 unsigned index =0)
    : SndAnyControl(card, Find(card, name, iface, index),
		    SND_CTL_ELEM_TYPE_IEC958, "IEC958") { if (isReadable()) read(); }

  SndIec958Control(const class SndCard* card,
		   snd_hctl_elem_t* elem)
    : SndAnyControl(card, elem,
		    SND_CTL_ELEM_TYPE_IEC958, "IEC958") { if (isReadable()) read(); }

  void read(void) override
  {
    SndAnyControl::read(
      [](snd_ctl_elem_value_t* ctl, unsigned idx){
	snd_aes_iec958_t data;
	snd_ctl_elem_value_get_iec958(ctl, &data);
	return data;
      });
  }

  void write(void) override
  {
    SndAnyControl::write(
      [](snd_ctl_elem_value_t* ctl, unsigned idx, snd_aes_iec958_t value){
	snd_ctl_elem_value_set_iec958(ctl, &value);
      },
      [] (snd_aes_iec958_t v) {return true;});
  }
};
