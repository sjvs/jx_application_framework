/******************************************************************************
 test_jFileUtils.cpp

	Program to test file & directory utils.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jFStreamUtil.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(File)
{
	JString testFileName;
	JAssertOK(JCreateTempFile(&testFileName));

	JAssertTrue(JNameUsed(testFileName));
	JAssertTrue(JFileExists(testFileName));
	JAssertTrue(JFileReadable(testFileName));
	JAssertTrue(JFileWritable(testFileName));
	JAssertFalse(JFileExecutable(testFileName));
	JAssertFalse(JDirectoryExists(testFileName));

	JSize size;
	JAssertOK(JGetFileLength(testFileName, &size));
	JAssertEqual(0, size);

	JAssertOK(JRemoveFile(testFileName));
	JAssertFalse(JFileExists(testFileName));
	JAssertFalse(JFileWritable(testFileName));

	std::ofstream file(testFileName.GetBytes());
	file << "abc";
	file.close();
	JAssertTrue(JFileExists(testFileName));
	JAssertTrue(JFileWritable(testFileName));

	JAssertOK(JGetFileLength(testFileName, &size));
	JAssertEqual(3, size);

	JString path, name;
	JAssertTrue(JSplitPathAndName(testFileName, &path, &name));

	JString s = JGetUniqueDirEntryName(path, JString("test_j_file_util_", kJFalse));
	JAssertOK(JRenameFile(testFileName, s));
	JAssertFalse(JFileExists(testFileName));
	JAssertTrue(JFileExists(s));

	JAssertOK(JRemoveFile(s));
}

JTEST(Symlink)
{
	JString f1("data/test_JString.txt", kJFalse);
	JString f2("test_jFileUtil.dat", kJFalse);

	JAssertTrue(JFileExists(f1));
	if (JFileExists(f2))
		{
		JAssertOK(JRemoveFile(f2));
		}
	JAssertOK(JCreateSymbolicLink(f1, f2));
	JAssertTrue(JSameDirEntry(f1, f2));

	JString f3;
	JAssertOK(JGetSymbolicLinkTarget(f2, &f3));
	JAssertTrue(JSameDirEntry(f1, f3));

	JAssertOK(JRemoveFile(f2));
}

JTEST(Permissions)
{
	JAssertStringsEqual("rwxr-xr-x", JGetPermissionsString(0755));
}

JTEST(Directory)
{
	JString dirName("code", kJFalse);
	JAssertFalse(JFileExists(dirName));
	JAssertTrue(JDirectoryExists(dirName));
	JAssertTrue(JDirectoryWritable(dirName));
	JAssertTrue(JCanEnterDirectory(dirName));

	JString d1("junk", kJFalse);
	JAssertOK(JCreateDirectory(d1));

	JString d2("junk2", kJFalse);
	JAssertOK(JRenameDirectory(d1, d2));

	JAssertOK(JRemoveDirectory(d2));

	JString path;
	JAssertOK(JCreateTempDirectory(nullptr, nullptr, &path));
	JAssertOK(JRemoveDirectory(path));

	JAssertTrue(JGetTempDirectory(&path));

	JAssertTrue(JSearchSubdirs(
		JString(".", kJFalse),
		JString("test_JFileUtil.cpp", kJFalse),
		kJTrue, kJFalse, &path));
}

JTEST(Files)
{
	const JString path("/tmp/junk/junk2/junk3", kJFalse);
	JAssertOK(JCreateDirectory(path));

	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	for (JIndex i=1; i<=10; i++)
		{
		JString* fileName = jnew JString();
		assert( fileName != nullptr );

		JAssertOK(JCreateTempFile(&path, nullptr, fileName));
		fileList.Append(fileName);
		}

	JAssertOK(JRenameFile(*(fileList.GetFirstElement()), *(fileList.GetLastElement()), kJTrue));
	JAssertTrue(JKillDirectory(JString("/tmp/junk", kJFalse)));
}

JTEST(Path)
{
	JString s = JCombinePathAndName(
		JString("/usr", kJFalse), JString("local", kJFalse));
	JAssertStringsEqual("/usr/local", s);

	JAppendDirSeparator(&s);
	JAppendDirSeparator(&s);
	JAssertStringsEqual("/usr/local/", s);

	JString path, name;
	JAssertTrue(JSplitPathAndName(s, &path, &name));
	JAssertStringsEqual("/usr/", path);
	JAssertStringsEqual("local", name);

	JStripTrailingDirSeparator(&s);
	JAssertStringsEqual("/usr/local", s);

	s += "///";
	JStripTrailingDirSeparator(&s);
	JAssertStringsEqual("/usr/local", s);

	s = "//usr/./local/../local/";
	JAssertTrue(JGetTrueName(s, &name));
	JAssertStringsEqual("/usr/local/", name);

	JCleanPath(&s);
	JAssertStringsEqual("/usr/local/../local/", s);
	JAssertTrue(JIsAbsolutePath(s));

	s = JGetRootDirectory();
	JAssertTrue(JIsRootDirectory(s));

	s = "./code";
	JAssertTrue(JIsRelativePath(s));

	JAssertTrue(JConvertToAbsolutePath(s, nullptr, &path));
	s = JConvertToRelativePath(path, JGetCurrentDirectory());
	JAssertStringsEqual("./code", s);

	s = JConvertToRelativePath(
		JString("/usr/include", kJFalse),
		JString("/usr/local", kJFalse));
	JAssertStringsEqual("../include", s);

	s = "/usr";
	JAssertTrue(JConvertToAbsolutePath(s, nullptr, &path));
	JAssertStringsEqual("/usr", path);

	s = JGetClosestDirectory(
		JString("/usr/include/zzz/junk/foo/bar/baz.cc", kJFalse));
	JAssertStringsEqual("/usr/include/", s);

	s = "./test_j_file_util_test_directory";
	JAssertOK(JCreateDirectory(s));
	s = JGetClosestDirectory(
		JString("./test_j_file_util_test_directory/foo/bar/baz", kJFalse));
	JAssertStringsEqual("test_j_file_util_test_directory/", s);
	JAssertOK(JRemoveDirectory(s));
}

JTEST(ExtractFileAndLine)
{
	JString s;
	JIndex start, end;
	JExtractFileAndLine(JString("foo.cpp:5", kJFalse), &s, &start);
	JAssertEqual("foo.cpp", s);
	JAssertEqual(5, start);

	JExtractFileAndLine(JString("foo.cpp:7-12", kJFalse), &s, &start, &end);
	JAssertEqual("foo.cpp", s);
	JAssertEqual(7, start);
	JAssertEqual(12, end);
}

JTEST(RootSuffix)
{
	JString s("foo.cpp", kJFalse);
	JString root, suffix;
	JAssertTrue(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo", root);
	JAssertStringsEqual("cpp", suffix);

	s = JCombineRootAndSuffix(root, suffix);
	JAssertStringsEqual("foo.cpp", s);

	s = "foo.5";
	JAssertFalse(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo.5", root);
	JAssertTrue(suffix.IsEmpty());

	s = "foo.";
	JAssertFalse(JSplitRootAndSuffix(s, &root, &suffix));
	JAssertStringsEqual("foo.", root);
	JAssertTrue(suffix.IsEmpty());
}

JTEST(Url)
{
	JString s("./test_jFileUtil", kJFalse), fullName;
	JAssertTrue(JConvertToAbsolutePath(s, nullptr, &fullName));

	s = JFileNameToURL(fullName);

	JString s2;
	JAssertTrue(JURLToFileName(s, &s2));
	JAssertStringsEqual(fullName, s2);
}

JTEST(HomeDirShortcut)
{
	JString home;
	JAssertTrue(JGetHomeDirectory(&home));

	const JString path = JCombinePathAndName(home, JString("test_j_file_util_test_directory", kJFalse));
	JAssertOK(JCreateDirectory(path));

	JString s = JConvertToHomeDirShortcut(path);
	JAssertStringsEqual("~/test_j_file_util_test_directory", s);

	JString s2;
	JAssertTrue(JConvertToAbsolutePath(s, nullptr, &s2));
	JAssertStringsEqual(path, s2);

	s = JGetClosestDirectory(
		JString("~/test_j_file_util_test_directory/foo/bar/baz", kJFalse),
		kJTrue);
	JAssertStringsEqual("~/test_j_file_util_test_directory/", s);

	JAssertOK(JRemoveDirectory(path));
}
