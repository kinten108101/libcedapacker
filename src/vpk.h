#ifndef ARCHIVAL_VPK_H
#define ARCHIVAL_VPK_H

/**
 * 
 * dir:  path
 * path: path
 * 
 * /home/johndoe/mates-vpk/src/archival
 * ├─ common_utils.c
 * ├─ vpk.h
 * 
 * file: vpk.h (in full english it should be called "id")
 * name: vpk
 * ext:  h
 * dir:  src/archival
 * workingdir: /home/johndoe/mates-vpk
 * path: src/archival/vpk.h
 * abs_path:    /home/johndoe/mates-vpk/src/archival/vpk.h
 * 
 * /home/johndoe/mates-vpk/addons/guns/p90-kriegs
 * ├─ p90-kriegs-latest.vpk
 * ├─ ADDONBUILD
 * 
 * moddir:  guns/p90-kriegs
 * pakfile:  p90-kriegs-latest.vpk
 * pakpath: guns/p90-kriegs/p90-kriegs-latest.vpk
 * name_addonsdir: addons
 * 
 * 
 * # package space
 * 
 * the package aka the vpk
 * ├─ the directory aka vpkdir aka x_dir.vpk aka package's metadata
 * │  ├─ header aka vpkdirheader
 * │  ├─ tree
 * │  │  ├─ entry aka vpkdirentry
 * │  │  ...
 * │  ├─ (footer)
 * ├─ the archive (x_*.vpk) package's raw content
 * 
 * 
 * # archive space
 * 
 * /
 * ├─ materials/
 * │  ├─ vgui/
 * │     ├─ hud/
 * │        ├─ icon_smg.vmt
 * │        ├─ icon_smg.vtf
 * ├─ addoninfo.txt
 * ├─ addonimage.jpg
 * 
 * inarchive_file:  icon_smg.vmt
 * inarchive_fname: icon_smg
 * inarchive_fext:  vmt
 * inarchive_fdir:  materials/vgui/hud
 * inarchive_fpath: materials/vgui/hud/icon_smg.vmt
 */

typedef struct VpkDirHeader
{
	const unsigned int sig;
	const unsigned int ver;
	const unsigned int tree_size;
} VpkDirHeader;

typedef struct VpkDirEntry
{
	const unsigned int crc; // A 32bit crc of the file's data.
	const unsigned short preload_size; // The number of bytes contained in the index file.
	// A zero based index of the archive this file's data is contained in.
	// If 0x7fff, the data follows the f_dir.
	const unsigned short archive_idx;
	// If archive_idx is 0x7fff, the offset of the file data relative to the end of the f_dir (see the header for more details).
	// Otherwise, the offset of the data from the start of the specified archive.
	const unsigned int entry_offset;
	// If zero, the entire file is stored in the preload data.
	// Otherwise, the number of bytes stored starting at entry_offset.
	const unsigned int entry_size;
	const unsigned short Terminator;
} VpkDirEntry;

typedef struct VpkFile VpkFile;
struct VpkFile
{
	const char* ext;
	const char* name;
	const char* dir;
};

typedef struct hashlist
{
	// max 2^16=65536, whereas there are at most
	// 1000 files in a common vpk. We use MAX_FILES 2048
	unsigned short int len;
	unsigned short int** list;
	// byte size: 10 or 16 instead of 20 (stack overflow)
} hashlist;
// Pascal integer array. A string that starts with a len counter.
// Cost 4 more bytes but blazingly fast.

#endif
