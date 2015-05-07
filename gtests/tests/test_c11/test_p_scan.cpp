#include <gtest/gtest.h>
/*
在一篇英文文章中查找指定的人名，人名使用二十六个英文字母（可以是大写或小写）、空格以及两个通配符组成（*、 ? ），通配符“*”表示零个或多个任意字母，通配符“ ? ”表示一个任意字母。
如：“J* Smi ? ? ” 可以匹配“John Smith” .
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory>

int _strncmp(const char * pszText, const char* pszName, int n){
    int i = 0;
    for (i = 0; i < n && pszName[i] && pszText[i]; i++){
        if (pszText[i] == pszName[i])
            continue;
        if (isalpha(pszText[i]) &&  pszName[i] == '?')
            continue;
        break;
    }
    return n-i;
}

const char * _strstr(const char* pszText, const char * pszName,int n){
    for (pszText; *pszText; pszText++){
        if (0 == _strncmp(pszText, pszName, n)){
            return pszText;
        }
    }
    return NULL;
}

//返回m的位置，或者结尾0的位置
const char* _strchar(const char* pszName, char m){
    for (pszName; *pszName && *pszName != m; *pszName++){}
    return pszName;
}


const char* _strscan(const char* pszText , const char* pszName){

    const char* word_end = pszName;
    const char* str = NULL;
    const char* first = NULL;
    const char* last = NULL;

    while (*word_end && *pszText){
        //忽略连续的*
        if (*word_end == '*'){
            word_end++;
            pszName++;
            first = first ? first : pszText;

            if (*word_end == 0){
                while (*pszText++) {}
                last = pszText;
            }
            continue;
        }

        word_end = _strchar(pszName, '*');//*end ==0 代表已经匹配到匹配串尾
        int n = word_end - pszName;
        str = _strstr(pszText, pszName, n);
        if(str){
            first = first ? first : str;
            pszName += n;
            pszText = str+n;
            last = str + n;
        }
        else{
            //failed
            return NULL;
        }
        
    };

    if (*word_end == 0){
        char * tmp = (char*)malloc(last - first + 1);
        strncpy(tmp, first, last - first);
        tmp[last - first] = 0;
        return tmp;
    }
    
    return NULL;


    
    
}

TEST(canTest, MatchNameInText) {
    EXPECT_STREQ(_strscan("adbdfsadf", "f?a"),"fsa");
    EXPECT_STREQ(_strscan("sdfJohn Smith", "J* Smi??"), "John Smith");
    EXPECT_STREQ(_strscan("dsfasJohn Smith", "J* Smi*"), "John Smith");
    EXPECT_STREQ(_strscan("John Smith", "*Smi*"), "John Smith");
    EXPECT_STREQ(_strscan("JohJohn Smith", "J?hn*Smi*"), "John Smith");
    EXPECT_STREQ(_strscan("John Smith", "*Sma*"), NULL);
    
}

