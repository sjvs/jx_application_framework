#include <jTypes.h>
static const JUtf8Byte* kJCoreDefaultStringData[] = {
"0\nJTooManyDescriptorsOpen \"The program is not allowed to open any more files.\"\nJCantRenameFileToDirectory \"You cannot rename \\\"$old_name\\\" to \\\"$new_name\\\" because the former is a file while the latter is a directory\"\nJUnknownError \"An unknown error ($err) occurred.\"\nWarnExpire::jTime \"This copy of $name $version will expire on $date.\n\nIf you have found $name to be useful, please consider purchasing one or more licenses from New Planet Software before the expiration date so you can continue to use this software without interruption.\n\n$url\n$email\"\nWrongVersion::JPrefsManager \"The preferences file has the wrong version.\"\nJPathContainsLoop \"\\\"$name\\\" contains a loop caused by symbolic links.\"\nJProgramNotAvailable \"The program \\\"$name\\\" either does not exist or is not in any of the directories on the current execution path.\"\nJNotSymbolicLink \"\\\"$name\\\" is not a symbolic link.\"\nWrongSite::jSysUtil \"This copy of $name is only for use on computers that are part of $site.\n\nYou can obtain a usable copy of this program from:\n\n$url\"\nLoneDollar::JSubstitute \"The dollar symbol ($) must be followed by a variable name.\"\nExpired::jTime \"This copy of $name $version has expired.\n\nIf you have found $name to be useful, please consider purchasing one or more licenses from New Planet Software so you can continue to use it.\n\n$url\n$email\"\nJNoError \"No errors occurred.\"\nJNonBlockedSignal \"A non-blocked signal was received which interrupted the operation.\"\nFileIsNotXPM::JImage \"\\\"$name\\\" does not contain an XPM.\"\nJDirectoryCantBeOwnChild \"A directory cannot be renamed so that it becomes a sub-directory of itself.\"\nJNoProcessMemory \"There is not enough free memory to complete the operation.\"\nFileNotWritable::JFileArray \"\\\"$name\\\" cannot be opened because you do not have permission to write to it.\"\nJDeviceFull \"A write was attempted to a device with insufficient room to accept the data.\"\nTrailingBackslash::JSubstitute \"The backslash sym",
"bol (\\\\) must be followed by a character.\"\nWrongSignature::JFileArray \"\\\"$name\\\" has the wrong signature.\"\nFileIsNotPNG::JImage \"\\\"$name\\\" does not contain a PNG.\"\nFileIsNotJPEG::JImage \"\\\"$name\\\" does not contain a JPEG.\"\nPNGNotAvailable::JImage \"The PNG format is not supported.\"\nJPathContainsLoop2 \"At least one of \\\"$name1\\\" or \\\"$name2\\\" contains a loop caused by symbolic links.\"\nJDescriptorNotOpen \"At least one of the file descriptors does not refer to an open file.\"\nJTriedToRemoveDirectory \"remove() can only remove files, not directories.\"\nJDirectoryNotEmpty \"\\\"$name\\\" cannot be removed because it is not empty.\"\nJBadPath \"\\\"$name\\\" is invalid.\"\nJGeneralIO \"A general I/O error (errno=EIO) occured.\"\nJNameTooLong \"The specified pathname is too long.\"\nJNegativeDescriptor \"At least one of the file descriptors is negative.\"\nIllegalControlChar::JSubstitute \"The control character sequence (\\\\c) must be followed by a character in the ASCII table between A and underscore (_) inclusive.\"\nNoHomeDirectory::JPrefsFile \"Unable to create a preferences file because you don't have a home directory.\"\nJWouldHaveBlocked \"The requested operation would have blocked and the stream is set to non-blocking.\"\nJNoHomeDirectory \"You do not have a home directory.\"\nJUnexpectedError \"An unexpected error ($err) occured -- please contact the developer so the situation can be accommodated in the future.\"\nJSegFault \"Recoverable segmentation fault: the programmer screwed up!\"\nJInvalidDescriptor \"At least one of the file descriptors was invalid.\"\nJCantRenameToNonemptyDirectory \"You cannot rename a directory to an existing, non-empty directory.\"\nJComponentNotDirectory2 \"Some part of \\\"$name1\\\" or \\\"$name2\\\" is not a directory.\"\nJTooManyLinks \"Renaming \\\"$name\\\" would exceed the maximum allowed number of file system links.\"\nJNotCompressed \"\\\"$name\\\" is not compressed.\"\nJFileSystemFull \"The file system does not have enough ",
"free space left.\"\nJAccessDenied \"You are not allowed to create or access \\\"$name\\\".\"\nJFileSystemReadOnly \"You cannot write to a read-only file system.\"\nJDirEntryDoesNotExist \"\\\"$name\\\" does not exist.\"\nJIsADirectory \"An operation which is only legal on a file was attempted on a directory.\"\nJDirEntryAlreadyExists \"\\\"$name\\\" already exists.\"\nJAccessDenied2 \"You are not allowed to create or access at least one of \\\"$name1\\\" or \\\"$name2\\\".\"\nJPEGNotAvailable::JImage \"The JPEG format is not supported.\"\nTooManyColors::JImage \"The image contains too many colors.\"\nFileAlreadyOpen::JFileArray \"\\\"$name\\\" is already open.\"\nFileIsNotGIF::JImage \"\\\"$name\\\" does not contain a GIF.\"\nJComponentNotDirectory \"Some part of \\\"$name\\\" is not a directory.\"\nJBadPath2 \"At least of one \\\"$name1\\\" or \\\"$name2\\\" is invalid.\"\nJDirectoryBusy \"\\\"$name\\\" cannot be removed because it is being used by another program.\"\nGIFNotAvailable::JImage \"The GIF format is not supported.\"\nJInvalidOpenMode \"The requested method of opening \\\"$name\\\" is invalid.\"\nJFileBusy \"\\\"$name\\\" cannot be replaced because it is being used by another program.\"\nJNoKernelMemory \"There is not enough free memory for the kernel to complete the operation.  (Your computer is in big trouble!)\"\nJMemoryNotReadable \"An attempt was made to read from memory without read permission.\"\nJCantRenameAcrossFilesystems \"You cannot rename a file so that it moves to a different file system.\"\nFileIsNotXBM::JImage \"\\\"$name\\\" does not contain an XBM.\"\nNotEmbeddedFile::JFileArray \"The specified element ($index) is not an embedded JFileArray.\"\nJMemoryNotWriteable \"An attempt was made to write to memory without write permission.\"\nUnknownFileType::JImage \"\\\"$name\\\" either does not contain an image or uses an supported format.\"\n",
NULL };
