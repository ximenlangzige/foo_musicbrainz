#pragma once

struct Track
{
	str8 artist;
	str8 artistid;
	str8 releasetrackid;
	str8 title;
	str8 trackid;
	str8 track;
	str8 totaltracks;
};

struct Disc
{
	bool is_various;
	pfc::list_t<Track> tracks;
	str8 disc;
	str8 format;
	str8 subtitle;
	str8 totaldiscs;
};

struct Release
{
	pfc::list_t<Disc> discs;
	str8 album_artist;
	str8 albumartistid;
	str8 albumid;
	str8 asin;
	str8 barcode;
	str8 catalognumber;
	str8 country;
	str8 date;
	str8 discid;
	str8 first_release_date;
	str8 label;
	str8 language;
	str8 primary_type;
	str8 releasegroupid;
	str8 script;
	str8 status;
	str8 title;
};

static const char* release_group_types[] = {
	"(None)",
	"Album",
	"Single",
	"EP",
	"Compilation",
	"Soundtrack",
	"Spokenword",
	"Interview",
	"Audiobook",
	"Live",
	"Remix",
	"Other"
};

static const char* release_statuses[] = {
	"(None)",
	"Official",
	"Promotion",
	"Bootleg",
	"Pseudo-Release"
};

static const char* ascii_replacements[][2] = {
	{ "…", "..." },
	{ "‘", "'" },
	{ "’", "'" },
	{ "‚", "'" },
	{ "“", "\"" },
	{ "”", "\"" },
	{ "„", "\"" },
	{ "′", "'" },
	{ "″", "\"" },
	{ "‹", "<" },
	{ "›", ">" },
	{ "«", "\"" },
	{ "»", "\"" },
	{ "‐", "-" },
	{ "‒", "-" },
	{ "–", "-" },
	{ "−", "-" },
	{ "—", "-" },
	{ "―", "-" }
};

static const t_size ascii_replacements_count = PFC_TABSIZE(ascii_replacements);

Release parser(json release, t_size handle_count);
str8 get_status_str(t_size idx);
str8 get_type_str(t_size idx);
str8 slasher(const str8& one, const str8& two);
str8 to_str(json j);
t_size get_status_index(str8 str);
t_size get_type_index(str8 str);
void ascii_replacer(str8& out);
void get_artist_credit(json j, str8& name, str8& id);
void tagger(metadb_handle_list_cref handles, Release release, t_size disc_idx);
