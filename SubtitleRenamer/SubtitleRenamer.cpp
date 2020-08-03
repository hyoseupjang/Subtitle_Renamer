#include <algorithm>
#include <Windows.h>
#include <io.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cctype>
#include <sstream>
#include <string>
#include <regex>
using namespace std;
bool compareNat(const string& a, const string& b)
{
	if (a.empty())
		return true;
	if (b.empty())
		return false;
	if (isdigit(a[0]) && !isdigit(b[0]))
		return true;
	if (!isdigit(a[0]) && isdigit(b[0]))
		return false;
	if (!isdigit(a[0]) && !isdigit(b[0]))
	{
		if (toupper(a[0]) == toupper(b[0]))
			return compareNat(a.substr(1), b.substr(1));
		return (toupper(a[0]) < toupper(b[0]));
	}

	// Both strings begin with digit --> parse both numbers
	istringstream issa(a);
	istringstream issb(b);
	int ia, ib;
	issa >> ia;
	issb >> ib;
	if (ia != ib)
		return ia < ib;

	// Numbers are the same --> remove numbers and recurse
	string anew, bnew;
	getline(issa, anew);
	getline(issb, bnew);
	return (compareNat(anew, bnew));
}
vector<string> rmDuplicate(vector<string>& a, vector <string>& b)
{
	vector<string>::iterator iter;
	vector<string>::iterator iter_b;
	vector<string> c = a; //a의 값 복사

	for (iter_b = b.begin(); iter_b != b.end(); iter_b++) {
		for (iter = c.begin(); iter != c.end();) {
			if ((*iter).substr(0, (*iter).find_last_of(".")) == (*iter_b).substr(0, (*iter_b).find_last_of(".")))
			{
				iter = c.erase(iter); //중복 제거
			}
			else
			{
				iter++;
			}
		}
	}
	return c; //결과 반환
}
vector <string> rawList, mediaFullName, subtitleFullName, rdMediaFullName, rdSubtitleFullName, changedSubtitleName;
//rawList 전체 파일.확장자
//mediaFullName 미디어파일.확장자
//rdMediaFullName 미디어파일.확장자 (중복 제거)
//rdSubtitleFullName 자막파일.확장자 (중복 제거)
//subtitleFullName 자막파일.확장자
//mediaName 미디어파일
//subtitleName 자막파일
int main()
{
	_finddata64i32_t currentDir;
	regex mediaExtPat("\\.(?:mkv|MKV|mp4|MP4|avi|AVI)$");
	regex subtitleExtPat("\\.(?:smi|SMI|ass|ASS|srt|SRT)$");
	smatch ext;
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
	long handle;
	int renameResult;
	handle = _findfirst64i32("*.*", &currentDir);
	if (_findfirst64i32("*.*", &currentDir) == -1)
	{
		exit(0); //파일 없음
	}
	for (int i = 1; i != -1; i= _findnext64i32(handle, &currentDir))
	{
		rawList.push_back(currentDir.name);
	}
	for (int i=0;i < rawList.size(); i++)
	{
		if (regex_search(rawList[i], mediaExtPat))
		{
			mediaFullName.push_back(rawList[i]);
		}
		if (regex_search(rawList[i], subtitleExtPat))
		{
			subtitleFullName.push_back(rawList[i]);
		}
	}
	if (mediaFullName.size() * subtitleFullName.size() == 0)
	{
		exit(0); //둘중 하나가 없음
	}
	rdMediaFullName = rmDuplicate(mediaFullName, subtitleFullName);
	sort(rdMediaFullName.begin(), rdMediaFullName.end(), compareNat);
	rdSubtitleFullName = rmDuplicate(subtitleFullName, mediaFullName);
	sort(rdSubtitleFullName.begin(), rdSubtitleFullName.end(), compareNat); //중복 제거후 정렬
	if (rdMediaFullName.size() * rdSubtitleFullName.size() == 0)
	{
		exit(0); //둘중 하나가 없음
	}
	if (rdMediaFullName.size() != rdSubtitleFullName.size())
	{
		MessageBoxA(NULL, "영상 파일의 개수와 자막 파일의 개수가 다릅니다. \n프로그램은 종료되었습니다. \n자막 파일과 영상 파일의 이름에는 어떠한 수정도 가해지지 않았습니다. ", "경고", MB_OK | MB_ICONEXCLAMATION);
		exit(0); //미디어-자막 개수 다름
	}
	for (int i = 0; i < rdMediaFullName.size(); i++)
	{
		regex_search(rdSubtitleFullName[i], ext, subtitleExtPat);
		changedSubtitleName.push_back(rdMediaFullName[i].substr(0, rdMediaFullName[i].find_last_of(".")) + ext[0].str());
	}
	for (int i = 0; i < rdSubtitleFullName.size(); i++)
	{
		renameResult = rename(rdSubtitleFullName[i].c_str(), changedSubtitleName[i].c_str());
		if (renameResult == -1)
		{
			char errmsg[1024];
			strerror_s(errmsg, 1024, errno);
			MessageBoxA(NULL, errmsg, "오류", MB_OK | MB_ICONEXCLAMATION);
			exit(1);
		}
	}
}