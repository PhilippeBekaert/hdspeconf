# AIO Pro configuration

![AIO Pro control panel](AioProTCO.png "AIO Pro control panel")

Left-to-right, top-to-bottom:

**Clock Source**

Current clock source: "Master" for master mode. "Internal" if not master but no valid external synchronisation reference. "Word Clk", "AES", "S/PDIF", "ADAT", "TCO" or "Sync In" otherwise.

**Sample Rate**

Current effective sound card sample rate in samples per second. May differ slightly from standard rates (32000, 44100, 48000, 64000, 88200, 96000, 128000, 176400, 192000) for external clock source, or due to non-unit pitch setting. If non-standard, this fields changes color into warn-full orange.

**Buffer Size**

PCM data is handed from the sound card to the system or vice versa in buffers of this many samples. Together with the sample rate, the buffer size determines the latency between incoming and outgoing PCM data. At 48000Hz, a buffer size of 128 samples leads to 2.7 milliseconds capture latency. Playback latency is twice that on linux.  A smaller buffer size leads to lower latency and vice versa. But a small buffer size also increases audio card interrupt rate and, if too low, will cause X-Runs (buffer under- or overruns), leading to clicking noises. On modern systems, buffers sizes as small as 64 samples will generally work well. The minimum is 32 samples. The maximum is 4096 samples. Buffer size is set by linux ALSA during PCM initialisation. It cannot be set using hdspeconf.

**FW version**

Sound card firmward version.

**Preferred Clock Source / Input Status**

The radio buttons enable to set the preferred clock source. Select "Internal" to use the sound card in clock master mode. If the selected preferred clock source is not available or valid, the sound card will use a next clock source. The actual clock source is reported in the **Clock Source** field.

For each potential clock source, status is indicated: 
- "N/A" means the source hardware (Word Clock module, if a TCO card is present, and TCO otherwise) is not available. The card does not report whether or not an optional Word Clock Module is present or not. If neither TCO or WCM are present, Word Clock status will show as "No Lock". 
- "No Lock" mean no valid signal is detected on the source. 
- "Lock" means the source hardware is available and a valid signal is detected on it, but it is not compatible with the current clock source.
- "Sync" means the source hardware is available and a valid signal is detected on it, compatible with the current clock source.
While being live, it is safe to switch preferred clock source to a source with "Sync" status. Switching to other clock source may result in sudden sample rate changes and clicks and noises.

The panel also indicates the detected sample rate on external sources with valid signal ("Lock" or "Sync" status), and warns about sources with sample rate not compatible with the current source by displaying a warning sign.

The internal sample rate is set by linux ALSA during PCM initialization. It cannot be set using hdspeconf is some audio application is running. hdspeconf only allows changing the sample rate if no application is running. The setting may be overridden by ALSA tools after, though.

