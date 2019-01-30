#define MAXBLOCKSIZE 1024
#include <stdio.h>
#include <wininet.h>
#include<string.h>
/**
 * 利用wininet函数库，发送http请求并保存网页文件
 * libraries 要添加配置wininet
 */
int main3()
{

    GetWebSrcCode("http://list.jd.com/list.html?cat=670,671,672");
    return 0;
}
void GetWebSrcCode(const char *Url)
{
    HINTERNET hSession = InternetOpen("zwt", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hSession != NULL)
    {
        HINTERNET hURL = InternetOpenUrl(hSession, Url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
        if (hURL != NULL)
        {
            char Temp[MAXBLOCKSIZE] = {0};
            ULONG Number = 1;

            FILE * stream;
            if( (stream = fopen( "d:/GetWebPage.html", "wb" )) != NULL )
            {
                while (Number > 0)
                {
                    InternetReadFile(hURL, Temp, MAXBLOCKSIZE - 1, &Number);
                    fwrite(Temp, sizeof (char), Number ,stream);
                }
                fclose(stream );
            }

            InternetCloseHandle(hURL);
            hURL = NULL;
        }

        InternetCloseHandle(hSession);
        printf("over!");
        hSession = NULL;
    }
}


