#include "stdafx.h"
#include "toc.h"
#include "sha1.h"

namespace mb
{
	toc::toc(metadb_handle_list_cref p_data)
	{
		num_tracks = p_data.get_count();
		pregap = 150;
		discid = nullptr;
		cur_track = 0;
		tracks_lengths = new unsigned int[num_tracks];
		t_int64 samples;
		size_t spf = p_data.get_item(0)->get_info_ref()->info().info_get_int("samplerate") == 48000 ? 640 : 588;

		for (size_t i = 0; i < num_tracks; i++)
		{
			samples = p_data.get_item(i)->get_info_ref()->info().info_get_length_samples();
			if (i == 0)
			{
				const char* tmp = p_data.get_item(i)->get_info_ref()->info().info_get("pregap");
				if (tmp != nullptr)
				{
					set_pregap(tmp);
				}
			}
			tracks_lengths[cur_track++] = (unsigned int)samples / spf;
		}

		calculate_tracks();
	}

	toc::~toc()
	{
		delete[] tracks_lengths;
		if (discid) delete[] discid;
	}

	void toc::set_pregap(str8 msf)
	{
		regex rx("^\\d\\d:\\d\\d:\\d\\d$");
		if (regex_match(msf.get_ptr(), rx))
		{
			pregap += (((msf[0] - '0') * 10 + (msf[1] - '0')) * 60 + (msf[3] - '0') * 10 + (msf[4] - '0')) * 75 + (msf[6] - '0') * 10 + (msf[7] - '0');
		}
	}

	void toc::calculate_tracks()
	{

		tracks[1] = pregap;
		for (unsigned int i = 2; i < num_tracks + 1; i++)
		{
			tracks[i] = tracks[i - 1] + tracks_lengths[i - 2];
		}
		tracks[0] = tracks[num_tracks] + tracks_lengths[num_tracks - 1];
		for (int i = num_tracks + 1; i < 100; i++)
		{
			tracks[i] = 0;
		}
	}

	char* toc::get_discid()
	{
		if (discid == nullptr)
		{
			SHA1Context sha;
			SHA1Reset(&sha);
			char tmp[9];
			unsigned char digest[20];
			unsigned long discid_length;

			sprintf_s(tmp, "%02X", 1);
			SHA1Input(&sha, (unsigned char*)tmp, 2);

			sprintf_s(tmp, "%02X", num_tracks);
			SHA1Input(&sha, (unsigned char*)tmp, 2);

			for (int i = 0; i < 100; i++)
			{
				sprintf_s(tmp, "%08X", tracks[i]);
				SHA1Input(&sha, (unsigned char*)tmp, 8);
			}

			SHA1Result(&sha, digest);
			discid = (char*)rfc822_binary(digest, 20, discid_length);
			discid[discid_length] = '\0';
		}

		return discid;
	}

	const char* toc::get_toc()
	{
		if (toc_str.is_empty())
		{
			char tmp[9];
			toc_str << "1";
			sprintf_s(tmp, " %d", num_tracks);
			toc_str << tmp;
			for (size_t i = 0; i <= num_tracks; i++)
			{
				sprintf_s(tmp, " %d", tracks[i]);
				toc_str << tmp;
			}
		}

		return toc_str.get_ptr();
	}

	/*
	 * Program:	RFC-822 routines (originally from SMTP)
	 */

	 /* NOTE: This is not true RFC822 anymore. The use of the characters
		'/', '+', and '=' is no bueno when the ID will be used as part of a URL.
		'_', '.', and '-' have been used instead
	 */

	 /* Convert binary contents to BASE64
	  * Accepts: source
	  *	    length of source
	  *	    pointer to return destination length
	  * Returns: destination as BASE64
	  */

	unsigned char* toc::rfc822_binary(void* src, unsigned long srcl, unsigned long& len)
	{
		unsigned char* ret;
		unsigned char* d;
		unsigned char* s = (unsigned char*)src;
		char* v = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._";
		len = ((srcl + 2) / 3) * 4;
		d = ret = new unsigned char[len + 1];
		while (srcl)
		{ /* process tuplets */
			*d++ = v[s[0] >> 2];	/* byte 1: high 6 bits (1) */
					/* byte 2: low 2 bits (1), high 4 bits (2) */
			*d++ = v[((s[0] << 4) + (--srcl ? (s[1] >> 4) : 0)) & 0x3f];
			/* byte 3: low 4 bits (2), high 2 bits (3) */
			*d++ = srcl ? v[((s[1] << 2) + (--srcl ? (s[2] >> 6) : 0)) & 0x3f] : '-';
			/* byte 4: low 6 bits (3) */
			*d++ = srcl ? v[s[2] & 0x3f] : '-';
			if (srcl) srcl--;		/* count third character if processed */
			s += 3;
		}
		*d = '\0';			/* tie off string */

		return ret;			/* return the resulting string */
	}
}
