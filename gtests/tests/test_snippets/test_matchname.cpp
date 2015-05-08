#include "gtest/gtest.h"



/*
��Ѷ���ֳ���Ƹ����Ŀ��
��һƪӢ�������в���ָ��������������ʹ�ö�ʮ����Ӣ����ĸ�������Ǵ�д��Сд�����ո��Լ�����ͨ�����ɣ�*��?����ͨ�����*����ʾ�������������ĸ��ͨ�����?����ʾһ��������ĸ��
�磺��J* Smi??�� ����ƥ�䡰John Smith�� .
void scan(const char* pszText, const char* pszName);
ע��pszTextΪ���������ַ���pszNameΪҪ��ƥ���Ӣ������
����ɺ���ʵ���������ƥ���Ӣ����������printf�⣬�����õ������Ŀ⺯���ȡ�
------------------------------------------
���������������ģ��� ����kmp�Ӵ�������ƥ�䣩���ԼӸ��죬�� abcabd*?abe**??de����ܣ����ǿ��Էֳ�abcabd,?,abe,?,?,����˳����ƥ��abcabd����ƥ��󣬽�ƥ��������е�ַ ��ƥ����Ǻ��Ӵ��ŵ�ջ���¼������������ÿ��ƥ�䶼��ջ���浱ǰ�Ӵ�ƥ����Ϣ����һ��������ȫ���Ӵ���ƥ����󣬾����һ��ƥ������Ȼ�����һ�£��� ʼ��ջ�����Ӵ��Ľ���������һ����ʼλ�õ�ƥ�䡣

�ҵĿ����������ģ���һ��ϸ�ڱ��ἰ�ıȽ��٣�Ҫ��ƥ������������Կո�ָ��������ַ�����


*/

#include <stdio.h>

static bool _isalpha(char c){
    return  (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool _isalphanum(char c){
    return  _isalpha(c) || (c >= '0' && c <= '9');
}

static bool _isspace(char c){
    return c == 0x20 || c == '\t';
}

static bool _match(const char* s, const char* m, const char** _end = NULL){

    while (*s && *m){
        switch (*m)
        {
        case '*':
            //���*�����һ������ôƥ��
            m++;

            //��һ��ģʽ�ַ��ǿհ�
            if (_isspace(*m)){
                //ƥ���굱ǰ����
                while (_isalpha(*s)){ s++; }
                continue;
            }

            //*�Ѿ���ģʽ�ַ��������һ���ַ��ˣ�over
            if (*m == 0){
                while (!_isalpha(*s)){ s++; }
                if (_end) *_end = s;
                return true;
            }

            /*�ӳֵݹ�ģʽ��ע��ֻ��Ҫƥ�䵱ǰword*/
            for (s; *s && _isalpha(*s); s++){
                if (_match(s, m, _end))
                    return true;
            }
            return false;
            break;
        case '?':
            if (!_isalpha(*s))
                return false;
            break;
        case ' ':
        case '\t'://ƥ��հ�
            if (!_isspace(*s))
                return false;
            //���������հ�
            while (_isspace(*(m + 1))) { m++; }
            while (_isspace(*(s + 1))) { s++; }
            break;
        default://alpha and space

            if (*m != *s)
                return false;
            break;
        }
        s++, m++;

    }

    if (!*m && !_isalphanum(*s)){
        if (_end) *_end = s;
        return true;
    }

    return false;
}


const char* _strdump(const char* start, const char* end){
    char name[1024];
    char * t = name;
    while (start < end) { *t++ = *start++; }
    *t = 0;
    return name;

}

int _scan(const char* pszText, const char* pszName){
    int i = 0;

    for (const char* s = pszText; *s;){
        const char * _start = NULL, *_end = NULL;

        //find a start
        while (*s && !_isalpha(*s)){ s++; }
        _start = s;
        if (_match(s, pszName, &_end)){
            printf("%s\n", _strdump(_start, _end));
            s = _end;
            i++;
        }
        else{
            while (*s && _isalpha(*s)){ s++; }
        }

    }
    return i;
}

//class MatchNameTest : public ::testing::Test {
//protected:
//    virtual void SetUp() {
//
//    }
//
//    virtual void TearDown() {}
//
//public:
//};


TEST(MatchNameTest, dummy) {
    //    EXPECT_TRUE(match_token("John", 4, "J*", 2));
    //EXPECT_TRUE(match_token("Smith",5, "Smi??",5));
    EXPECT_TRUE(_match("John", "J*"));
    EXPECT_TRUE(_match("Smith", "Smi??"));
    EXPECT_FALSE(_match("John Smith", "j* Smi??"));
    EXPECT_TRUE(_match("John Smith", "J* Smi??"));
    EXPECT_FALSE(_match("John Smith1", "J* Smi??"));

    EXPECT_FALSE(_match("John Jack Smith", "J* Smi??"));
    EXPECT_FALSE(_match("John John1 Smith", "J* Smi??"));
    EXPECT_FALSE(_match(",John Zhang Smith.", "J*h* S*i??"));
    EXPECT_FALSE(_match(",JohnSmith.", "J*h* S*i??"));

    EXPECT_TRUE(_match("John Smith Tom", "J* Smi?? Tom"));
    EXPECT_TRUE(_match("John    Smith Tom", "J* Smi?? Tom"));
    EXPECT_TRUE(_match("John Smith Tom", "J* Smi??     Tom"));
    EXPECT_TRUE(_match("John       Smith         Tom", "J* Smi??     Tom"));

    EXPECT_EQ(_scan("John Smith", "J* Smi??"), 1);

    EXPECT_EQ(_scan(", John Smith", "J* Smi??"), 1);
    EXPECT_EQ(_scan(", John Smith,c,John Smith.", "J* Smi??"), 2);

    EXPECT_EQ(_scan(", John Smith,c,John Smith.", "J*h* S*i??"), 2);

    EXPECT_EQ(_scan(", John Smith,John Zhang,John Smith.", "J*h* S*i??"), 2);

    EXPECT_EQ(_scan(", John Smith,John Zhang John John Smith,John Smith.", "J*h* S*i??"), 3);
    EXPECT_EQ(_scan(", John Smith,John Zhang John1 John Smith,John Smith.", "J*h* S*i??"), 3);
    EXPECT_EQ(_scan(", John Smith,John Zhang Smith,John Smith.", "J*h* S*i??"), 2);
}
