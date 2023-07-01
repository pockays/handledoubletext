#include <iostream>
#include <string>
#include <fstream>
#include <codecvt>
#include <Windows.h>

using namespace std;


wstring StrToWStr(string& strString, UINT uCodePage)
{
	int lenWStr = 0;
	wstring result;

	lenWStr = MultiByteToWideChar(uCodePage, NULL, strString.c_str(), strString.size(), NULL, NULL);
	wchar_t* buffer = new wchar_t[lenWStr + 1];
	MultiByteToWideChar(uCodePage, NULL, strString.c_str(), strString.size(), buffer, lenWStr);
	buffer[lenWStr] = '\0';

	result.append(buffer);
	delete[] buffer;
	return result;
}
bool DumpText(string strFileName)
{
	auto cvtUTF8 = locale(locale::empty(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode(consume_header | generate_header | little_endian)>);
	wofstream oText(strFileName + ".txt");
	oText.imbue(cvtUTF8);
	ifstream iScript(strFileName);
	if (iScript.is_open() && oText.is_open())
	{
		size_t lineCount = 0;
		for (string perLine; getline(iScript, perLine); lineCount++)
		{
			if (perLine.size() <= 0)
			{
				continue;
			}

			if (((unsigned char)perLine[0] >= (unsigned char)0x7B) ||
				perLine.find("CS \"") == 0 ||
				perLine.find("SELECT \"") != string::npos
				)
			{
				oText << L"LineCount:" << lineCount << '\n';
				oText << L"Raw:" << StrToWStr(perLine, 932) << '\n';
				oText << L"Tra:" << StrToWStr(perLine, 932) << '\n' << '\n';
			}
		}

		oText.flush();
		oText.close();
		iScript.close();
		return true;
	}

	return false;
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: " << argv[0] << " <filename>" << endl;
		return 1;
	}
	string strFileName = argv[1];
	DumpText(strFileName);
	return 0;
}