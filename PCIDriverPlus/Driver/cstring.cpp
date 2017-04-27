/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "windrvr.h"
#include "cstring.h"

#define INITIAL_CSTRING_SIZE 1024
#define MAX_CSTRING_SIZE (unsigned long)1024*256

CCString::CCString()
{
    Init();
}

CCString::CCString(PCSTR pcStr)
{
    Init();
    *this = pcStr;
}

CCString::CCString(const CCString& stringSrc)
{
    Init();
    *this = stringSrc;
}

void CCString::Init()
{
    m_str = NULL;
    GetBuffer(INITIAL_CSTRING_SIZE);
}

const CCString& CCString::operator= (PCSTR pcStr)
{
    if (pcStr)
    {
        unsigned long len = (unsigned long)strlen(pcStr);

        GetBuffer(len);
        strcpy (m_str, pcStr);
    }
    else
        *m_str = '\0';
    return *this;
}

int CCString::Length()
{
    if (!m_str)
        return 0;
    return (int)strlen(m_str);
}

int CCString::is_empty()
{
    return !m_str || !m_str[0];
}

CCString::operator PCSTR() const
{
    return m_str;
}

CCString::operator char *() const
{
    return m_str;
}

CCString::~CCString()
{
    if (m_str)
        free(m_str);
}

const int CCString::operator==(const CCString& stringSrc)
{
    return Compare(stringSrc)==0;
}

const int CCString::operator==(const char *stringSrc)
{
    return Compare(stringSrc)==0;
}

const int CCString::operator!=(const CCString& stringSrc)
{
    return Compare(stringSrc)!=0;
}

const int CCString::operator!=(const char *stringSrc)
{
    return Compare(stringSrc)!=0;
}

const CCString& CCString::operator=(const CCString& stringSrc)
{
    *this = stringSrc.m_str;
    return *this;
}

const CCString& CCString::operator+=(const PCSTR pcStr)
{
    if (pcStr)
    {
        unsigned long len = (unsigned long)strlen(pcStr);

        GetBuffer(Length() + len);
        strcat (m_str, pcStr);
    }
    return *this;
}

const CCString& CCString::cat_printf(const PCSTR format, ...)
{
    if (format)
    {
        CCString tmp;

        va_list ap;
        va_start(ap, format);
        tmp.vsprintf(format, ap);
        va_end(ap);

        GetBuffer(Length() + tmp.Length());
        strcat(m_str, tmp.m_str);
    }
    return *this;
}

const CCString operator+(const CCString &str1, const CCString &str2)
{
    CCString tmp(str1);
    tmp += str2;
    return tmp;
}

char& CCString::operator[](int i)
{
    return m_str[i];
}

int CCString::Compare(PCSTR psStr)
{
    if (psStr)
        return ::strcmp(m_str, psStr);
    return Length();
}

#if defined(WIN32)
    #define vsnprintf _vsnprintf
#endif

void CCString::Format(const PCSTR format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsprintf(format, ap);
    va_end(ap);
}

int CCString::IsAllocOK()
{
    return TRUE;
}

// len is the needed maximum number of characters in string, excluding the terminating NULL
int CCString::GetBuffer(unsigned long len)
{
    if (!m_str)
        m_buf_size = 0;

    // add the terminating NULL and translate to bytes
    int nNeeded = (int) len+1;
    if (m_buf_size>=nNeeded)
        return TRUE;

    int nNewSize = nNeeded > INITIAL_CSTRING_SIZE ? nNeeded : INITIAL_CSTRING_SIZE;
    char *tmpBuffer = (char *) malloc (nNewSize);
    if (!tmpBuffer) // Error, Not enough memory
        return FALSE;
    tmpBuffer[0] = '\0';

    if (m_str)
    {
        memcpy(tmpBuffer, m_str, m_buf_size);
        free(m_str);
    }
    m_buf_size = nNewSize;
    m_str = tmpBuffer;

    return TRUE;
}

#if !defined(WIN32)
int stricmp(PCSTR str1, PCSTR str2)
{
    int i;
    for (i=0; str1[i] && str2[i]; i++)
       if (toupper(str1[i])!=toupper(str2[i]))
           return -1;
    return str1[i] || str2[i] ? -1 : 0;
}
#endif

CCString CCString::Mid(int nFirst, int nCount)
{
    CCString sRet;
    if (nFirst>=Length())
        return sRet;
    sRet = (m_str + nFirst);
    if (nCount<Length())
        sRet.m_str[nCount-nFirst] = '\0';
    return sRet;
}

CCString CCString::Mid(int nFirst)
{
    CCString sRet;
    if (nFirst>=Length())
        return sRet;
    sRet = (m_str + nFirst);
    return sRet;
}

CCString CCString::StrRemove(PCSTR str)
{
    char *found;
    CCString sRet;
    CCString s1, s2;
    
    if ((found = strstr(m_str, str)) == NULL)
        return *this;
  
    s1 = this->Mid(0, found - m_str);
    s2 = this->Mid(found - m_str + strlen(str));
    sRet = s1 + s2;
    return sRet;
}

int CCString::CompareNoCase(PCSTR str)
{
    return this->stricmp(str);
}

void CCString::MakeUpper()
{
    int i;

    for (i=0; (UINT) i<strlen(m_str); i++)
        m_str[i] = ::toupper(m_str[i]);
}

void CCString::MakeLower()
{
    int i;

    for (i=0; (UINT) i<strlen(m_str); i++)
        m_str[i] = ::tolower(m_str[i]);
}

void CCString::sprintf(const PCSTR format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsprintf(format, ap);
    va_end(ap);
}

void CCString::vsprintf(const PCSTR format, va_list ap)
{
    size_t i;
    CCString tmp;
    va_list ap1;
    int rc = -1;

    for (i = INITIAL_CSTRING_SIZE; i < MAX_CSTRING_SIZE && rc < 0;
        i += INITIAL_CSTRING_SIZE)
    {
        tmp.GetBuffer(i);
        
        va_copy(ap1, ap);
        rc = vsnprintf(tmp.m_str, i - 1, format, ap1);
        va_end(ap1);
    }
    *this = tmp;
}

int CCString::strcmp(const PCSTR s)
{
    if (s)
        return ::strcmp(m_str, s);
    return Length();
}

int CCString::stricmp(const PCSTR s)
{
    if (s)
        return ::stricmp(m_str, s);
    return Length();
}

void CCString::toupper()
{
    MakeUpper();
}

void CCString::tolower()
{
    MakeLower();
}

const int CCString::operator!=(char *s)
{
    return Compare(s)!=0;
}

CCString CCString::tolower_copy(void) const
{
    CCString tmp(m_str);
    tmp.tolower();
    return tmp;
}

CCString CCString::toupper_copy(void) const
{
    CCString tmp(m_str);
    tmp.toupper();
    return tmp;
}

