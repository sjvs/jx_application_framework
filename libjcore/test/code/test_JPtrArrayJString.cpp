/******************************************************************************
 test_JPtrArrayJString.cc

	Program to test JPtrArray<JString>.

	Written by John Lindal.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <JPtrArray-JString.h>
#include <sstream>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(ReadWritePtrArray)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	list2.Append(JString("foobar", 0, kJFalse));

	std::stringstream io1;
	io1 << list1;
	io1.seekg(0);
	io1 >> list2;
	JAssertTrue(list2.IsEmpty());

	list1.Append(JString("foobar", 0, kJFalse));
	std::stringstream io2;
	io2 << list1;
	io2.seekg(0);
	io2 >> list2;
	JAssertEqual(1, list2.GetElementCount());
	JAssertStringsEqual("foobar", *(list2.GetElement(1)));

	list1.Prepend(JString("barbaz", 0, kJFalse));

	std::stringstream io3;
	io3 << list1;
	io3.seekg(0);
	io3 >> list2;
	JAssertEqual(2, list2.GetElementCount());
	JAssertStringsEqual("barbaz", *(list2.GetElement(1)));
	JAssertStringsEqual("foobar", *(list2.GetElement(2)));
}

JTEST(ReadWritePtrMap)
{
	JStringPtrMap<JString> map1(JPtrArrayT::kDeleteAll),
						   map2(JPtrArrayT::kDeleteAll);

	const JString key1("a", 0, kJFalse),
				  key2("z", 0, kJFalse);

	JString* s;

	map2.SetNewElement(key1, JString("foobar", 0, kJFalse));

	std::stringstream io1;
	io1 << map1;
	io1.seekg(0);
	io1 >> map2;
	JAssertTrue(map2.IsEmpty());

	map1.SetNewElement(key1, JString("foobar", 0, kJFalse));
	std::stringstream io2;
	io2 << map1;
	io2.seekg(0);
	io2 >> map2;
	JAssertEqual(1, map2.GetElementCount());
	JAssertTrue(map2.GetElement(key1, &s));
	JAssertStringsEqual("foobar", *s);

	map1.SetNewElement(key2, JString("barbaz", 0, kJFalse));

	std::stringstream io3;
	io3 << map1;
	io3.seekg(0);
	io3 >> map2;
	JAssertEqual(2, map2.GetElementCount());
	JAssertTrue(map2.GetElement(key1, &s));
	JAssertStringsEqual("foobar", *s);
	JAssertTrue(map2.GetElement(key2, &s));
	JAssertStringsEqual("barbaz", *s);
}

JTEST(JSameStrings)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	JAssertTrue(JSameStrings(list1, list2, kJFalse));
	JAssertTrue(JSameStrings(list1, list2, kJTrue));

	list1.Append(JString("foobar", 0, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJTrue));

	list2.Append(JString("barbaz", 0, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJTrue));

	list2.SetElement(1, JString("foobar", 0, kJFalse), JPtrArrayT::kDelete);
	JAssertTrue(JSameStrings(list1, list2, kJFalse));
	JAssertTrue(JSameStrings(list1, list2, kJTrue));

	list1.Append(JString("barbaz", 0, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJTrue));

	list2.Append(JString("barbaz", 0, kJFalse));
	JAssertTrue(JSameStrings(list1, list2, kJFalse));
	JAssertTrue(JSameStrings(list1, list2, kJTrue));

	list2.SetElement(1, JString("FOOBAR", 0, kJFalse), JPtrArrayT::kDelete);
	JAssertTrue(JSameStrings(list1, list2, kJFalse));
	JAssertFalse(JSameStrings(list1, list2, kJTrue));
}

JTEST(Join)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

	JAssertStringsEqual("", list.Join(","));

	list.Append(JString("foobar", 0, kJFalse));
	JAssertStringsEqual("foobar", list.Join(","));

	list.Append(JString("barbaz", 0, kJFalse));
	JAssertStringsEqual("foobar,barbaz", list.Join(","));

	JAssertStringsEqual("foobarbarbaz", list.Join(""));
}