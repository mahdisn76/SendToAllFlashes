#include<iostream>
#include<windows.h>
#include<tchar.h>
#include<shellapi.h>
#include<vector>
#include<QThread>
#include<string>
using namespace std;
int copy_directory(wchar_t*src,wchar_t*dest)
{
	SHFILEOPSTRUCTW x ;
	x.pFrom = src;
	x.pTo=dest;
	x.wFunc=FO_COPY;
	x.hwnd=NULL;
	x.fFlags = FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION;
	return SHFileOperationW(&x);
}

void set_src(int argc,char*argv[],wchar_t*src) //after each directory it have to be a 0
											//and
											//at the end it must be two zero
{
	int t=0;
	for(int i=1;i<argc;i++)
	{
		int len_=strlen(argv[i]);
		for(int j=0;j<len_;j++)
		{
			src[t]=argv[i][j];
			t++;
		}
		src[t]=0;
		t++;
	}
	src[t]=0;
	return;
}

class mythread : public QThread
{
public :
	wchar_t*src=nullptr;
	wchar_t*dest=nullptr;
	mythread (wchar_t*v_src,wchar_t*v_dest)
		:src(v_src),
		  dest(v_dest)
	{
	}

	void run ()
	{
		copy_directory(src,dest);
	}

};

int main(int argc, char* argv[])
{
	int src_length=0;
	for(int i=0;i<argc;i++)		//calculating length of src
		src_length+=strlen(argv[i]+1);
	src_length+=1;				// for the ending zero

	wchar_t *src = new wchar_t[src_length+10];
	if(argc>=2)
	{
		vector<mythread*> thrs;
		vector<wchar_t*> dests;

		set_src(argc,argv,src);

		TCHAR p[] = TEXT("A:\\");
		for(TCHAR d='B';d<='Z';d++)
		{
			p[0]=d;
			if (GetDriveType(p) == DRIVE_REMOVABLE)
			{
				wchar_t* dest = new wchar_t[5];
				dest[0]=d;
				dest[1]=':';
				dest[2]='\\';
				dest[3]=0;
				dest[4]=0;
				dests.push_back(dest);

				thrs.push_back(new mythread(src,dest));
				thrs.back()->start();
			}
		}
		for(unsigned int i=0;i<thrs.size();i++)
			if(thrs[i]->isRunning())
				thrs[i]->wait();

		for(unsigned int i=0;i<dests.size();i++)
			delete dests[i];
		delete src;
	}
	return 0;
}
