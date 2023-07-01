#include <iostream>
#include <string>
#include <fstream>
#include <codecvt>
#include <Windows.h>
#include <vector>

using namespace std;

string WStrToStr(wstring& wstrString, UINT uCodePage)
{
	int lenStr = 0;
	string result;

	lenStr = WideCharToMultiByte(uCodePage, NULL, wstrString.c_str(), wstrString.size(), NULL, NULL, NULL, NULL);
	char* buffer = new char[lenStr + 1];
	WideCharToMultiByte(uCodePage, NULL, wstrString.c_str(), wstrString.size(), buffer, lenStr, NULL, NULL);
	buffer[lenStr] = '\0';

	result.append(buffer);
	delete[] buffer;
	return result;
}

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

bool InsertText(string strFileName)
{
	auto cvtUTF8 = locale(locale::empty(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode(consume_header | generate_header | little_endian)>);

	vector<wstring> textLineList; //���ڴ������
	vector<unsigned int> textLinePositionList; //���ڴ�����Ķ�Ӧԭʼ�ű�������

	wifstream iText(strFileName + ".txt");
	iText.imbue(cvtUTF8);
	if (iText.is_open())
	{
		unsigned int textLinePosition = 0;
		for (wstring perLine; getline(iText, perLine);)
		{
			if (perLine.find(L"LineCount:") != wstring::npos)
			{
				//��ʵ����scanf���ѣ�ֻ�����ڴ����sscanf�����ֽڵĽ�swscanf
				swscanf_s(perLine.c_str(), L"LineCount:%d", &textLinePosition);
				textLinePositionList.push_back(textLinePosition);
				continue;
			}

			if (perLine.find(L"Tra:") != wstring::npos)
			{
				//�����Tra:���ĸ��ַ�ȥ���ˣ���Ϊ���ĸ��ַ�Ҳ��������ġ�
				textLineList.push_back(perLine.substr(0x4));
				continue;
			}
		}
		iText.close();
	}

	vector<string> scriptLineList;
	ifstream iScript(strFileName);
	if (iScript.is_open())
	{
		for (string perLine; getline(iScript, perLine);)
		{
			scriptLineList.push_back(perLine);
		}
		iScript.close();
	}


	ofstream oScript(strFileName + ".new");
	if (oScript.is_open())
	{
		size_t position = 0;
		wstring line;
		for (size_t iterPosition = 0; iterPosition < textLinePositionList.size(); iterPosition++)
		{
			position = textLinePositionList[iterPosition]; //ȡ��λ����Ϣ
			line = textLineList[iterPosition]; //ȡ������
			scriptLineList[position] = WStrToStr(line, 936);
		}

		for (auto& iteLine : scriptLineList)
		{
			oScript << iteLine << endl;
		}
		oScript.flush();
		oScript.close();
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
	InsertText(strFileName);
	return 0;
}
