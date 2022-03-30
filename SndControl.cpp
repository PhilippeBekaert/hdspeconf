/*! \file SndControl.cpp
 *! \brief ALSA sound card control element C++ wrappers.
 * 20210810,12,0902,03,04,06,12 - Philippe.Bekaert@uhasselt.be */

#include <iostream>

#include "SndControl.h"
#include "SndCard.h"

snd_hctl_elem_t* SndControl::Find(const class SndCard* card,
				  const std::string& name,
				  Interface iface,
				  unsigned index)
{
  snd_ctl_elem_id_t *id;
  snd_ctl_elem_id_alloca(&id);
  snd_ctl_elem_id_set_name(id, name.c_str());
  snd_ctl_elem_id_set_interface(id, (snd_ctl_elem_iface_t)iface);
  snd_ctl_elem_id_set_index(id, index);

  return Find(card, id);
}

snd_hctl_elem_t* SndControl::FindFromAsciiId(const class SndCard* card,
					     const std::string& asciiId)
{
  snd_ctl_elem_id_t *id;
  snd_ctl_elem_id_alloca(&id);
  if (snd_ctl_ascii_elem_id_parse(id, asciiId.c_str()) < 0)
    throw std::runtime_error("Failed to parse control element ASCII identifier '"
			     + asciiId + "'.\n");

  return Find(card, id);  
}

snd_hctl_elem_t* SndControl::Find(const class SndCard* card,
				  snd_ctl_elem_id_t* id)
{
  snd_hctl_elem_t* elem = snd_hctl_find_elem(card->hctl, id);  
  if (!elem) {
    const std::string name = std::string(snd_ctl_elem_id_get_name(id));
    snd_ctl_elem_iface_t iface = snd_ctl_elem_id_get_interface(id);
    unsigned index = snd_ctl_elem_id_get_index(id);

    throw std::runtime_error("No "
			     + std::string(snd_ctl_elem_iface_name(iface))
			     + " control element '" 
			     + std::string(name)
			     + "' index " + std::to_string(index)
			     + " found on card '"
			     + std::string(card->getName()) + "'\n");
  }

  return elem;
}

SndControl* SndControl::Create(const class SndCard* card,
			       snd_hctl_elem_t* elem)
{
  // get value type
  snd_ctl_elem_info_t *info;
  snd_ctl_elem_info_alloca(&info);
  SndCheckErr(snd_hctl_elem_info(elem, info), "hctl_elem_info");
  snd_ctl_elem_type_t type = snd_ctl_elem_info_get_type(info);

  switch (type) {
  case SND_CTL_ELEM_TYPE_BOOLEAN   :
    return new SndBoolControl(card, elem);
  case SND_CTL_ELEM_TYPE_INTEGER   :
    return new SndIntControl(card, elem);
  case SND_CTL_ELEM_TYPE_INTEGER64 :
    return new SndInt64Control(card, elem);
  case SND_CTL_ELEM_TYPE_ENUMERATED:
    return new SndEnumControl(card, elem);
  case SND_CTL_ELEM_TYPE_BYTES     :
    return new SndBytesControl(card, elem);
  case SND_CTL_ELEM_TYPE_IEC958    :
    return new SndIec958Control(card, elem);
  default:
    throw std::runtime_error("SndControl '"
			     + std::string(snd_hctl_elem_get_name(elem))
			     + "' on card '"
			     + std::string(card->getName())
			     + "' has unrecognized type "
			     + std::to_string(type) + ".\n");
  };
}

int SndControl::_elem_cb(snd_hctl_elem_t *elem, unsigned int mask)
{
  SndControl* c = (SndControl*) snd_hctl_elem_get_callback_private(elem);
  try {
    c->onElemEvent(mask);
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << "\n";
  } catch (...) {}
  return 0;
}

SndControl::SndControl(const class SndCard* _card,
		       snd_hctl_elem_t* _elem)
  : card(_card), elem(_elem)
{
  name = std::string(snd_hctl_elem_get_name(elem));

  // get value type and count
  snd_ctl_elem_info_malloc(&info);
  SndCheckErr(snd_hctl_elem_info(elem, info), "hctl_elem_info");
  type = snd_ctl_elem_info_get_type(info);
  count = snd_ctl_elem_info_get_count(info);
}

SndControl::~SndControl()
{
  snd_hctl_elem_set_callback(elem, nullptr);
  snd_ctl_elem_info_free(info);
}

void SndControl::checkChannel(unsigned i) const
{
  if (i >= count) {
    throw std::runtime_error("SndControl '" + name + "' on card '"
			     + getCardName() + "' channel index "
			     + std::to_string(i) + " out of range ("
			     + std::to_string(count) + " channels available).\n");
  }
}

const std::string SndControl::getCardName(void) const
{
  return card->getName();
}

const std::string SndControl::getAsciiId(void) const
{
  snd_ctl_elem_id_t* id;
  snd_ctl_elem_id_alloca(&id);  
  getId(id);
  return std::string(snd_ctl_ascii_elem_id_get(id));
}

int SndControl::getdBRange(long* min, long* max) const
{
  snd_ctl_elem_id_t* id;
  snd_ctl_elem_id_alloca(&id);  
  getId(id);
  return snd_ctl_get_dB_range(*card, id, min, max);
}

int SndControl::convertTodB(long volume, long* db_gain) const 
{
  snd_ctl_elem_id_t* id;
  snd_ctl_elem_id_alloca(&id);  
  getId(id);
  return snd_ctl_convert_to_dB(*card, id, volume, db_gain);
}

int SndControl::convertFromdB(long db_gain, long* volume, int xdir) const
{
  snd_ctl_elem_id_t* id;
  snd_ctl_elem_id_alloca(&id);  
  getId(id);
  return snd_ctl_convert_from_dB(*card, id, db_gain, volume, xdir);
}

bool SndControl::try_lock(void)
{
  snd_ctl_elem_id_t* id;
  snd_ctl_elem_id_alloca(&id);  
  getId(id);
  int rc = snd_ctl_elem_lock(*card, id);
  if (rc != 0 && rc != EBUSY)
    SndCheckErr(rc, "ctl_elem_lock");
  return rc == 0;
}

void SndControl::unlock(void)
{
  snd_ctl_elem_id_t* id;
  snd_ctl_elem_id_alloca(&id);  
  getId(id);
  SndCheckErr(snd_ctl_elem_unlock(*card, id), "ctl_elem_unlock");
}

#ifdef NEVER
//#include <stdio.h>
//#include <sys/types.h>

void SndControl::ValueLocker::lock(void)
{
  //  pthread_mutex_t* pmtx = ctl->rmtx.native_handle();
  if (!ctl->rmtx.try_lock()) {
    //    fprintf(stderr, "%s: thread %d waiting for lock held by %d.\n",
    //	    ctl->name.c_str(), gettid(), pmtx->__data.__owner);
    ctl->rmtx.lock();
    //    fprintf(stderr, "%s: thread %d locked.\n",
    //	    ctl->getName().c_str(), gettid());
  }
}

void SndControl::ValueLocker::unlock(void)
{
  ctl->rmtx.unlock();
  //  fprintf(stderr, "%.3f %s: %s thread %d unlocked.\n",
  //	  GetTime(), ctl->getName().c_str(), where, gettid());
}
#endif /*NEVER*/

