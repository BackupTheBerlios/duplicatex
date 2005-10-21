#include "htmlpage.h"

Chtmlpage::Chtmlpage(char * sourcepath, int filesize)
{
    int i, l;
    buffer = (char *) malloc(2049);
    GNPpars = (char *) malloc(2049);
    GNPparsFUNKTION = (char *) malloc(2049);
    GNPparsIF = (char *) malloc(2049);
    GNPparsSEARCH = (char *) malloc(2049);
    GNPparsTMP = (char *) malloc(2049);
    GNPparsVARNAME = (char *) malloc(2049);
    GNPreturn = (char *) malloc(2049);
    GNPreturnTITLE = (char *) malloc(2049);
    GNPparsP = 0;
    GNPloopEND = 0;
    GNPloopSTART = 0;
    GNPreturnCOUNTER = 0;
    targetfilename = (char *) malloc(1025);
    fileextension = (char *) malloc(33);
    textinfo = (char *) malloc(1025);
    parzeile = (char *) malloc(1025);
    httplinkurllogin = (char *) malloc(1025);
    httplinkurlpassword = (char *) malloc(1025);
    httplinkparlogin = (char *) malloc(1025);
    httplinkparpassword = (char *) malloc(1025);
    httplink = (char *) malloc(1025);
    httplinkurl = (char *) malloc(1025);
    httplinkpar = (char *) malloc(1025);
    cmd_a_title = NULL;
    cmd_a_href = NULL;
    if (strlen(sourcepath))
    {
        filename = (char *) malloc(strlen(sourcepath) + 1);
        strcpy(filename, sourcepath);
    }
    else
    {
        filename = (char *) malloc(1);
        filename[0] = 0;
        filesize = 0;
    }
    if (filesize)
    {
        fsize = filesize;
        fbuffer = (char *) malloc(fsize + 1);
        memset(fbuffer, 0, fsize + 1);
        wxFile * fpHtml = new wxFile(sourcepath, wxFile::read);
        i = 0;
        while ((!fpHtml -> Eof()) && (i < fsize))
        {
            l = 1024;
            if (l > (fsize - i))
            {
                l = fsize - i;
            }
            fpHtml -> Read(fbuffer + i, l);
            i += l;
        }
        delete fpHtml;
        fpHtml = NULL;
    }
    else
    {
        fsize = 0;
        fpHtml = NULL;
        fbuffer = NULL;
    }
    fbuffer_pointer = 0;
    for (i = 0 ; i < 1024 ; i++)
    {
        exblock[i] = NULL;
    }
    exblockAnz = 0;
    strcpy(buffer, "exblock.txt");
    if (!stat(buffer, statbuffer))
    {
        int bytecount, p, b;
        FILE * fp = fopen(buffer, "r");
        p = 0;
        bytecount = 0;
        while (!feof(fp))
        {
            b = fgetc(fp);
            if (b >= 0)
            {
                bytecount++;
                switch (b)
                {
                case 10:
                case 13:
                    if (p > 0)
                    {
                        p = - p;
                    }
                    break;
                case 9:
                case 32:
                    if (p)
                    {
                        buffer[p++] = b;
                    }
                    break;
                default:
                    buffer[p++] = b;
                    break;
                }
                if (p > 0)
                {
                    if (bytecount >= statbuffer -> st_size)
                    {
                        p = - p;
                    }
                }
                if (p < 0)
                {
                    p = - p;
                    buffer[p] = 0;
                    if (exblockAnz < 1024)
                    {
                        exblockAnz++;
                        exblock[exblockAnz] = (char *) malloc(strlen(buffer) + 1);
                        strcpy(exblock[exblockAnz], buffer);
                    }
                    p = 0;
                    buffer[0] = 0;
                }
            }
        }
        fclose(fp);
    }
}

Chtmlpage::~ Chtmlpage()
{
    int i = 0;
    while (i < exblockAnz)
    {
        i++;
        free(exblock[i]);
        exblock[i] = NULL;
    }
    free(buffer);
    free(fileextension);
    free(GNPpars);
    free(GNPparsFUNKTION);
    free(GNPparsIF);
    free(GNPparsSEARCH);
    free(GNPparsTMP);
    free(GNPparsVARNAME);
    free(GNPreturn);
    free(GNPreturnTITLE);
    free(filename);
    free(textinfo);
    free(parzeile);
    free(httplink);
    free(httplinkurllogin);
    free(httplinkurlpassword);
    free(httplinkurl);
    free(httplinkparlogin);
    free(httplinkparpassword);
    free(httplinkpar);
    free(targetfilename);
    if (fbuffer)
    {
        free(fbuffer);
        fbuffer = NULL;
    }
    if (cmd_a_title)
    {
        free(cmd_a_title);
        cmd_a_title = NULL;
    }
    if (cmd_a_href)
    {
        free(cmd_a_href);
        cmd_a_href = NULL;
    }
}

void Chtmlpage::getNextCommandline(char * commandline, int commandline_maxlength)
{
    int anfz, tiefe, tiefe0, letpointer;
    char b, b_last = 0;
    memset(commandline, 0, commandline_maxlength);
    memset(textinfo, 0, 1025);
    anfz = 0;
    tiefe = 0;
    tiefe0 = 0;
    cmdp = 0;
    txtp = 0;
    letpointer = 0;
    while (fbuffer_pointer < fsize)
    {
        b = fbuffer[fbuffer_pointer++];
        if (cmdp < (commandline_maxlength - 1))
        {
            if (!tiefe)
            {
                switch (b)
                {
                case '{':
                case '(':
                    if (!anfz)
                    {
                        if ((b_last >= 'a') && (b_last <= 'z'))
                        {
                            tiefe0++;
                        }
                        else if((b_last >= 'A') && (b_last <= 'Z'))
                        {
                            tiefe0++;
                        }
                    }
                    if (txtp < 1024)
                    {
                        textinfo[txtp++] = b;
                    }
                    break;
                case '}':
                case ')':
                    if (!anfz)
                    {
                        if (tiefe0)
                        {
                            tiefe0--;
                        }
                    }
                    if (txtp < 1024)
                    {
                        textinfo[txtp++] = b;
                    }
                    break;
                case '+':
                case '=':
                    if (txtp < 1024)
                    {
                        textinfo[txtp++] = b;
                    }
                    if (!anfz)
                    {
                        letpointer = 1;
                    }
                    break;
                case 9:
                case 10:
                case 13:
                case 32:
                    if (txtp)
                    {
                        if (b_last != 32)
                        {
                            if (txtp < 1024)
                            {
                                textinfo[txtp++] = b;
                            }
                        }
                    }
                    break;
                case 92:
                    b = fbuffer[fbuffer_pointer++];
                    if (txtp < 1024)
                    {
                        textinfo[txtp++] = b;
                    }
                    letpointer = 0;
                    break;
                case 34:
                    if (!anfz)
                    {
                        if (letpointer)
                        {
                            anfz = 1;
                        }
                    }
                    else if(anfz == 1)
                    {
                        anfz = 0;
                    }
                    if (txtp < 1024)
                    {
                        textinfo[txtp++] = b;
                    }
                    letpointer = 0;
                    break;
                case '<':
                    if ((!anfz) && (!tiefe0))
                    {
                        textinfo[txtp] = 0;
                        tiefe = 1;
                    }
                    else
                    {
                        if (txtp < 1024)
                        {
                            textinfo[txtp++] = b;
                        }
                    }
                    break;
                default:
                    if (txtp < 1024)
                    {
                        textinfo[txtp++] = b;
                    }
                    letpointer = 0;
                    break;
                }
            }
            else
            {
                switch (b)
                {
                case 9:
                case 10:
                case 13:
                    if (b_last != 32)
                    {
                        if (tiefe == 1)
                        {
                            commandline[cmdp++] = 32;
                        }
                    }
                    b = 32;
                    break;
                case 32:
                    if (tiefe == 1)
                    {
                        switch (anfz)
                        {
                        case 0:
                            if (cmdp)
                            {
                                if (b_last != 32)
                                {
                                    commandline[cmdp++] = 32;
                                }
                            }
                            break;
                        default:
                            commandline[cmdp++] = b;
                            break;
                        }
                    }
                    break;
                case '{':
                    if (!anfz)
                    {
                        tiefe++;
                    }
                    commandline[cmdp++] = b;
                    break;
                case '(':
                    if (!anfz)
                    {
                        tiefe++;
                    }
                    commandline[cmdp++] = b;
                    break;
                case '[':
                    commandline[cmdp++] = b;
                    break;
                case ')':
                    if (!anfz)
                    {
                        if (tiefe > 1)
                        {
                            tiefe--;
                        }
                    }
                    commandline[cmdp++] = b;
                    break;
                case '}':
                    if (!anfz)
                    {
                        tiefe--;
                    }
                    commandline[cmdp++] = b;
                    break;
                case ']':
                    commandline[cmdp++] = b;
                    break;
                case 92:
                    //backslash
                    commandline[cmdp++] = b;
                    commandline[cmdp++] = fbuffer[fbuffer_pointer++];
                    break;
                case 34:
                    if (anfz != 2)
                    {
                        anfz = 1 - anfz;
                    }
                    commandline[cmdp++] = b;
                    break;
                case '<':
                    commandline[cmdp++] = b;
                    break;
                case '>':
                    switch (anfz)
                    {
                    case 0:
                        if (tiefe == 1)
                        {
                            fsize = - fsize;
                        }
                        else
                        {
                            commandline[cmdp++] = b;
                        }
                        break;
                    default:
                        commandline[cmdp++] = b;
                        break;
                    }
                    break;
                default:
                    commandline[cmdp++] = b;
                    break;
                }
            }
        }
        else if(b == '>')
        {
            if (!anfz)
            {
                if (tiefe == 1)
                {
                    fsize = - fsize;
                }
            }
        }
        b_last = b;
    }
    commandline[cmdp] = 0;
    if (fsize < 0)
    {
        fsize = - fsize;
    }
    int l, p1, p2;
    l = strlen(commandline);
    p1 = 0;
    p2 = 0;
    while (p2 < l)
    {
        switch (commandline[p2])
        {
        case '_':
            if (!strncasecmp(commandline + p2, "_3a", 3))
            {
                commandline[p1] = ':';
                p2 += 2;
            }
            else if(!strncasecmp(commandline + p2, "_2f", 3))
            {
                commandline[p1] = '/';
                p2 += 2;
            }
            else if(!strncasecmp(commandline + p2, "_40", 3))
            {
                commandline[p1] = '@';
                p2 += 2;
            }
            else if(p1 < p2)
            {
                commandline[p1] = commandline[p2];
            }
            break;
        default:
            if (p1 < p2)
            {
                commandline[p1] = commandline[p2];
            }
            break;
        }
        p1++;
        p2++;
    }
    commandline[p1] = 0;
}

void Chtmlpage::clrTextinfo()
{
    memset(textinfo, 0, 1025);
}

char * Chtmlpage::getTextinfo()
{
    return textinfo;
}

int Chtmlpage::getTokenLast()
{
    return token_last;
}

int Chtmlpage::getTokenMain()
{
    return token_main;
}

char * Chtmlpage::getHttpLink()
{
    return httplink;
}

char * Chtmlpage::getHttpLinkUrl()
{
    return httplinkurl;
}

char * Chtmlpage::getHttpLinkPar()
{
    return httplinkpar;
}

void Chtmlpage::parseCommandline(char * htmlcode)
{
    int i, l, tiefe;
    int anfz, cmdp;
    char b, b_last, cmd[1025];
    memset(parzeile, 0, 1025);
    l = strlen(htmlcode);
    i = 0;
    anfz = 0;
    memset(cmd, 0, 1025);
    cmdp = 0;
    token = 0;
    token_last = 0;
    if (token_main < 0)
    {
        token_main = 0;
    }
    tiefe = 0;
    letpointer = 0;
    while (i <= l)
    {
        if (i == l)
        {
            b = 32;
        }
        else
        {
            b = htmlcode[i];
        }
        switch (b)
        {
        case '[':
            memset(cmd, 0, 1025);
            cmdp = 0;
            break;
        case '{':
        case '(':
            if (!anfz)
            {
                tiefe++;
            }
            cmd[cmdp++] = b;
            break;
        case '}':
        case ')':
            cmd[cmdp++] = b;
            //printf("tiefe=%d(%d): %s\n",tiefe,anfz,cmd);
            if (!anfz)
            {
                tiefe--;
                if (tiefe < 0)
                {
                    tiefe = 0;
                }
                if (!tiefe)
                {
                    memcpy(parzeile, cmd, 1025);
                    memset(cmd, 0, 1025);
                    cmdp = 0;
                }
            }
            break;
        case '=':
            if (!anfz)
            {
                token_last = token;
                token = getToken(cmd);
                switch (token)
                {
                case 2301:
                    //src
                case 2405:
                    //href
                    httplink[0] = 0;
                    httplinkurl[0] = 0;
                    httplinkpar[0] = 0;
                    break;
                case 2402:
                case 9999:
                    l = 0;
                    break;
                }
                memset(cmd, 0, 1025);
                cmdp = 0;
                letpointer = 1;
            }
            else
            {
                cmd[cmdp++] = b;
            }
            break;
        case '>':
        case 32:
        case ',':
            if (tiefe)
            {
                cmd[cmdp++] = b;
            }
            else if(anfz)
            {
                cmd[cmdp++] = b;
            }
            else if(cmdp)
            {
                token_last = token;
                if (letpointer)
                {
                    CMDexecute(cmd);
                    memset(cmd, 0, 1025);
                    cmdp = 0;
                    token = 0;
                    letpointer = 0;
                }
                else
                {
                    token = getToken(cmd);
                    switch (token)
                    {
                    case 401:
                        token_main = 0;
                        break;
                    case 101:
                    case 103:
                    case 201:
                    case 202:
                    case 301:
                    case 503:
                        httplink[0] = 0;
                        httplinkurl[0] = 0;
                        httplinkpar[0] = 0;
                        token_main = token;
                        break;
                    case 1201:
                    case 1203:
                    case 1301:
                    case 1302:
                    case 1401:
                    case 1603:
                        token_main = token;
                        break;
                    case 8999:
                        l = 0;
                        break;
                    case 2402:
                    case 9999:
                        l = 0;
                        break;
                    default:
                        //!token):
                        if (0)
                        {
                            printf("\nfilename:%s\n", filename);
                            printf("htmlcode(token=%d):%s\n", token, htmlcode);
                            printf("?%s?\n", cmd);
                            printf("_%s(tm=%d,t=%d,tl=%d)\n", cmd, token_main, token, token_last);
                            //getchar();
                        }
                        break;
                    }
                    memset(cmd, 0, 1025);
                    cmdp = 0;
                }
            }
            break;
        case ']':
            memset(cmd, 0, 1025);
            cmdp = 0;
            break;
        case 92:
            //backslash
            i++;
            cmd[cmdp++] = b;
            b = htmlcode[i];
            cmd[cmdp++] = b;
            printf("cmdline:%s\n", htmlcode);
            printf("cmd(%d):%s\n", b, cmd);
            break;
        case 34:
            cmd[cmdp++] = b;
            if (anfz != 2)
            {
                anfz = 1 - anfz;
                if (!anfz)
                {
                    if (cmd[0] == 34)
                    {
                        if (letpointer)
                        {
                            CMDexecute(cmd);
                        }
                        memset(cmd, 0, 1025);
                        cmdp = 0;
                        letpointer = 0;
                    }
                }
            }
            break;
        case 39:
            //hochkomma
            cmd[cmdp++] = b;
            if (anfz != 1)
            {
                anfz = 2 - anfz;
                if (!anfz)
                {
                    if (cmd[0] == '\'')
                    {
                        if (letpointer)
                        {
                            CMDexecute(cmd);
                        }
                        memset(cmd, 0, 1025);
                        cmdp = 0;
                        letpointer = 0;
                    }
                }
            }
            break;
        default:
            cmd[cmdp++] = b;
            break;
        }
        b_last = b;
        i++;
    }
    token_last = token;
}

void Chtmlpage::ClearAll()
{
    memset(httplink, 0, 1025);
    memset(httplinkurl, 0, 1025);
    memset(httplinkurllogin, 0, 1025);
    memset(httplinkurlpassword, 0, 1025);
    memset(httplinkpar, 0, 1025);
    memset(httplinkparlogin, 0, 1025);
    memset(httplinkparpassword, 0, 1025);
}

void Chtmlpage::CMDexecute(char * cmd)
{
    int it, it2, is, l, z, b, b_last = 0;
    switch (token)
    {
    case 2301:
        //src
    case 2405:
        //href
        {
            int p_last;
            memset(httplink, 0, 1025);
            memset(httplinkurllogin, 0, 1025);
            memset(httplinkurlpassword, 0, 1025);
            memset(httplinkurl, 0, 1025);
            memset(httplinkparlogin, 0, 1025);
            memset(httplinkparpassword, 0, 1025);
            memset(httplinkpar, 0, 1025);
            int urlloginz, parloginz;
            l = strlen(cmd);
            is = 0;
            z = 0;
            it = 0;
            it2 = 0;
            urlloginz = 0;
            parloginz = 0;
            p_last = 0;
            while (is <= l)
            {
                if (is == l)
                {
                    b = 0;
                }
                else
                {
                    b = cmd[is];
                }
                switch (b)
                {
                case '/':
                    if (it < 1024)
                    {
                        httplink[it++] = b;
                    }
                    if (b_last != b)
                    {
                        if (it2 < 1024)
                        {
                            switch (z)
                            {
                            case 0:
                                httplinkurl[it2++] = b;
                                p_last = it2;
                                break;
                            case 1:
                                httplinkpar[it2++] = b;
                                p_last = it2;
                                break;
                            }
                        }
                    }
                    break;
                case 32:
                case 34:
                case '\'':
                    break;
                case ':':
                    if (it < 1024)
                    {
                        httplink[it++] = b;
                    }
                    switch (z)
                    {
                    case 0:
                        if ((it2 > 3) && (!strncasecmp(httplinkurl + it2 - 4, "http", 4)))
                        {
                            memset(httplinkurl, 0, 1025);
                            it2 = 0;
                        }
                        else
                        {
                            httplinkurl[it2] = 0;
                            strcpy(httplinkurllogin, httplinkurl + p_last);
                            memset(httplinkurl + p_last, 0, 1025 - p_last);
                            it2 = p_last;
                            urlloginz = 1;
                        }
                        break;
                    case 1:
                        if ((it2 > 3) && (!strncasecmp(httplinkpar + it2 - 4, "http", 4)))
                        {
                            httplinkpar[it2] = 0;
                            memset(httplinkpar, 0, 1025);
                            it2 = 0;
                        }
                        else
                        {
                            httplinkpar[it2] = 0;
                            strcpy(httplinkparlogin, httplinkpar + p_last);
                            memset(httplinkpar + p_last, 0, 1025 - p_last);
                            it2 = p_last;
                            parloginz = 1;
                        }
                        break;
                    }
                    break;
                case 0:
                case '?':
                case '@':
                    if (it < 1024)
                    {
                        httplink[it++] = b;
                    }
                    switch (z)
                    {
                    case 0:
                        httplinkurl[it2] = 0;
                        if (urlloginz == 1)
                        {
                            strcpy(httplinkurlpassword, httplinkurl + p_last);
                            memset(httplinkurl + p_last, 0, 1025 - p_last);
                            it2 = p_last;
                            urlloginz = 2;
                            z--;
                        }
                        else
                        {
                            it2 = 0;
                        }
                        break;
                    case 1:
                        httplinkpar[it2] = 0;
                        if (parloginz == 1)
                        {
                            strcpy(httplinkparpassword, httplinkpar + p_last);
                            memset(httplinkpar + p_last, 0, 1025 - p_last);
                            it2 = p_last;
                            parloginz = 2;
                            z--;
                        }
                        else
                        {
                            it2 = 0;
                        }
                        break;
                    default:
                        it2 = 0;
                        break;
                    }
                    p_last = 0;
                    z++;
                    break;
                default:
                    {
                        if (it < 1024)
                        {
                            httplink[it++] = b;
                        }
                        if (it2 < 1024)
                        {
                            switch (z)
                            {
                            case 0:
                                httplinkurl[it2++] = b;
                                break;
                            case 1:
                                httplinkpar[it2++] = b;
                                break;
                            }
                        }
                    }
                    break;
                }
                is++;
                b_last = b;
            }
            httplink[it] = 0;
        }
        break;
    default:
        break;
    }
}

int Chtmlpage::getToken(char * cmdbuffer)
{
    int c, token = 0;
    c = cmdbuffer[0];
    switch (c)
    {
    case '+':
    case '-':
    case '#':
        token = 5000;
        break;
    case '?':
        //erweiterung
        token = 8999;
        break;
    case '\'':
        token = 9998;
        break;
    case '!':
        token = 9999;
        break;
    default:
        if ((c >= '0') && (c <= '9'))
        {
            token = 5000;
        }
        else
        {
            int l;
            char * tmp = strchr(cmdbuffer, 34);
            if (tmp)
            {
                if (tmp != cmdbuffer)
                {
                    tmp[0] = 0;
                }
            }
            l = strlen(cmdbuffer);
            switch (l)
            {
            case 1:
                {
                    if (!strcasecmp(cmdbuffer, "/"))
                    {
                        token = 1;
                    }
                    else if(!strcasecmp(cmdbuffer, "a"))
                    {
                        token = 101;
                    }
                    else if(!strcasecmp(cmdbuffer, "p"))
                    {
                        token = 102;
                    }
                    else if(!strcasecmp(cmdbuffer, "b"))
                    {
                        token = 103;
                    }
                    else if(!strcasecmp(cmdbuffer, "u"))
                    {
                        token = 104;
                    }
                    else if(!strcasecmp(cmdbuffer, "i"))
                    {
                        token = 105;
                    }
                }
                break;
            case 2:
                {
                    if (!strcasecmp(cmdbuffer, "tr"))
                    {
                        token = 201;
                    }
                    else if(!strcasecmp(cmdbuffer, "td"))
                    {
                        token = 202;
                    }
                    else if(!strcasecmp(cmdbuffer, "br"))
                    {
                        token = 203;
                    }
                    else if(!strcasecmp(cmdbuffer, "h1"))
                    {
                        token = 204;
                    }
                    else if(!strcasecmp(cmdbuffer, "em"))
                    {
                        token = 205;
                    }
                    else if(!strcasecmp(cmdbuffer, "h2"))
                    {
                        token = 206;
                    }
                    else if(!strcasecmp(cmdbuffer, "ul"))
                    {
                        token = 207;
                    }
                    else if(!strcasecmp(cmdbuffer, "li"))
                    {
                        token = 208;
                    }
                    else if(!strcasecmp(cmdbuffer, "th"))
                    {
                        token = 209;
                    }
                    else if(!strcasecmp(cmdbuffer, "h3"))
                    {
                        token = 210;
                    }
                    else if(!strcasecmp(cmdbuffer, "h4"))
                    {
                        token = 211;
                    }
                    else if(!strcasecmp(cmdbuffer, "h5"))
                    {
                        token = 212;
                    }
                    else if(!strcasecmp(cmdbuffer, "h6"))
                    {
                        token = 213;
                    }
                    else if(!strcasecmp(cmdbuffer, "tt"))
                    {
                        token = 214;
                    }
                    else if(!strcasecmp(cmdbuffer, "ol"))
                    {
                        token = 215;
                    }
                    else if(!strcasecmp(cmdbuffer, "dl"))
                    {
                        token = 216;
                    }
                    else if(!strcasecmp(cmdbuffer, "dt"))
                    {
                        token = 217;
                    }
                    else if(!strcasecmp(cmdbuffer, "dd"))
                    {
                        token = 218;
                    }
                    else if(!strcasecmp(cmdbuffer, "/a"))
                    {
                        token = 1201;
                    }
                    else if(!strcasecmp(cmdbuffer, "/p"))
                    {
                        token = 1202;
                    }
                    else if(!strcasecmp(cmdbuffer, "/b"))
                    {
                        token = 1203;
                    }
                    else if(!strcasecmp(cmdbuffer, "/u"))
                    {
                        token = 1204;
                    }
                    else if(!strcasecmp(cmdbuffer, "/i"))
                    {
                        token = 1205;
                    }
                    else if(!strcasecmp(cmdbuffer, "hr"))
                    {
                        token = 2201;
                    }
                    else if(!strcasecmp(cmdbuffer, "id"))
                    {
                        token = 2202;
                    }
                }
                break;
            case 3:
                {
                    if (!strcasecmp(cmdbuffer, "img"))
                    {
                        token = 301;
                    }
                    else if(!strcasecmp(cmdbuffer, "alt"))
                    {
                        token = 302;
                    }
                    else if(!strcasecmp(cmdbuffer, "div"))
                    {
                        token = 303;
                    }
                    else if(!strcasecmp(cmdbuffer, "pre"))
                    {
                        token = 304;
                    }
                    else if(!strcasecmp(cmdbuffer, "map"))
                    {
                        token = 305;
                    }
                    else if(!strcasecmp(cmdbuffer, "big"))
                    {
                        token = 306;
                    }
                    else if(!strcasecmp(cmdbuffer, "sub"))
                    {
                        token = 307;
                    }
                    else if(!strcasecmp(cmdbuffer, "sup"))
                    {
                        token = 308;
                    }
                    else if(!strcasecmp(cmdbuffer, "var"))
                    {
                        token = 309;
                    }
                    else if(!strcasecmp(cmdbuffer, "/tr"))
                    {
                        token = 1301;
                    }
                    else if(!strcasecmp(cmdbuffer, "/td"))
                    {
                        token = 1302;
                    }
                    else if(!strcasecmp(cmdbuffer, "/br"))
                    {
                        token = 1303;
                    }
                    else if(!strcasecmp(cmdbuffer, "/h1"))
                    {
                        token = 1304;
                    }
                    else if(!strcasecmp(cmdbuffer, "/em"))
                    {
                        token = 1305;
                    }
                    else if(!strcasecmp(cmdbuffer, "/h2"))
                    {
                        token = 1306;
                    }
                    else if(!strcasecmp(cmdbuffer, "/ul"))
                    {
                        token = 1307;
                    }
                    else if(!strcasecmp(cmdbuffer, "/li"))
                    {
                        token = 1308;
                    }
                    else if(!strcasecmp(cmdbuffer, "/th"))
                    {
                        token = 1309;
                    }
                    else if(!strcasecmp(cmdbuffer, "/h3"))
                    {
                        token = 1310;
                    }
                    else if(!strcasecmp(cmdbuffer, "/h4"))
                    {
                        token = 1311;
                    }
                    else if(!strcasecmp(cmdbuffer, "/h5"))
                    {
                        token = 1312;
                    }
                    else if(!strcasecmp(cmdbuffer, "/h6"))
                    {
                        token = 1313;
                    }
                    else if(!strcasecmp(cmdbuffer, "/tt"))
                    {
                        token = 1314;
                    }
                    else if(!strcasecmp(cmdbuffer, "/ol"))
                    {
                        token = 1315;
                    }
                    else if(!strcasecmp(cmdbuffer, "/dl"))
                    {
                        token = 1316;
                    }
                    else if(!strcasecmp(cmdbuffer, "/dt"))
                    {
                        token = 1317;
                    }
                    else if(!strcasecmp(cmdbuffer, "/dd"))
                    {
                        token = 1318;
                    }
                    else if(!strcasecmp(cmdbuffer, "src"))
                    {
                        token = 2301;
                    }
                }
                break;
            case 4:
                {
                    if (!strcasecmp(cmdbuffer, "html"))
                    {
                        token = 401;
                    }
                    else if(!strcasecmp(cmdbuffer, "head"))
                    {
                        token = 402;
                    }
                    else if(!strcasecmp(cmdbuffer, "body"))
                    {
                        token = 403;
                    }
                    else if(!strcasecmp(cmdbuffer, "font"))
                    {
                        token = 404;
                    }
                    else if(!strcasecmp(cmdbuffer, "form"))
                    {
                        token = 405;
                    }
                    else if(!strcasecmp(cmdbuffer, "span"))
                    {
                        token = 406;
                    }
                    else if(!strcasecmp(cmdbuffer, "nobr"))
                    {
                        token = 407;
                    }
                    else if(!strcasecmp(cmdbuffer, "code"))
                    {
                        token = 408;
                    }
                    else if(!strcasecmp(cmdbuffer, "cite"))
                    {
                        token = 409;
                    }
                    else if(!strcasecmp(cmdbuffer, "<</a"))
                    {
                        strcpy(textinfo + strlen(textinfo), "<<");
                        strcpy(cmdbuffer, "/a");
                        token = 1201;
                    }
                    else if(!strcasecmp(cmdbuffer, "/img"))
                    {
                        token = 1401;
                    }
                    else if(!strcasecmp(cmdbuffer, "/alt"))
                    {
                        token = 1402;
                    }
                    else if(!strcasecmp(cmdbuffer, "/div"))
                    {
                        token = 1403;
                    }
                    else if(!strcasecmp(cmdbuffer, "/pre"))
                    {
                        token = 1404;
                    }
                    else if(!strcasecmp(cmdbuffer, "/map"))
                    {
                        token = 1405;
                    }
                    else if(!strcasecmp(cmdbuffer, "/big"))
                    {
                        token = 1406;
                    }
                    else if(!strcasecmp(cmdbuffer, "/sub"))
                    {
                        token = 1407;
                    }
                    else if(!strcasecmp(cmdbuffer, "/sup"))
                    {
                        token = 1408;
                    }
                    else if(!strcasecmp(cmdbuffer, "/var"))
                    {
                        token = 1409;
                    }
                    else if(!strcasecmp(cmdbuffer, "name"))
                    {
                        token = 2401;
                    }
                    else if(!strcasecmp(cmdbuffer, "meta"))
                    {
                        token = 2402;
                    }
                    else if(!strcasecmp(cmdbuffer, "type"))
                    {
                        token = 2403;
                    }
                    else if(!strcasecmp(cmdbuffer, "link"))
                    {
                        token = 2404;
                    }
                    else if(!strcasecmp(cmdbuffer, "href"))
                    {
                        token = 2405;
                    }
                    else if(!strcasecmp(cmdbuffer, "face"))
                    {
                        token = 2406;
                    }
                    else if(!strcasecmp(cmdbuffer, "size"))
                    {
                        token = 2407;
                    }
                    else if(!strcasecmp(cmdbuffer, "text"))
                    {
                        token = 2408;
                    }
                    else if(!strcasecmp(cmdbuffer, "base"))
                    {
                        token = 2409;
                    }
                    else if(!strcasecmp(cmdbuffer, "area"))
                    {
                        token = 2410;
                    }
                    else if(!strcasecmp(cmdbuffer, "cols"))
                    {
                        token = 2411;
                    }
                    else if(!strcasecmp(cmdbuffer, "none"))
                    {
                        token = 2412;
                    }
                    else if(!strcasecmp(cmdbuffer, "styl"))
                    {
                        token = 5401;
                    }
                }
                break;
            case 5:
                {
                    if (!strcasecmp(cmdbuffer, "title"))
                    {
                        token = 501;
                    }
                    else if(!strcasecmp(cmdbuffer, "style"))
                    {
                        token = 502;
                    }
                    else if(!strcasecmp(cmdbuffer, "table"))
                    {
                        token = 503;
                    }
                    else if(!strcasecmp(cmdbuffer, "input"))
                    {
                        token = 504;
                    }
                    else if(!strcasecmp(cmdbuffer, "blink"))
                    {
                        token = 505;
                    }
                    else if(!strcasecmp(cmdbuffer, "small"))
                    {
                        token = 506;
                    }
                    else if(!strcasecmp(cmdbuffer, "label"))
                    {
                        token = 507;
                    }
                    else if(!strcasecmp(cmdbuffer, "/html"))
                    {
                        token = 1501;
                    }
                    else if(!strcasecmp(cmdbuffer, "/head"))
                    {
                        token = 1502;
                    }
                    else if(!strcasecmp(cmdbuffer, "/body"))
                    {
                        token = 1503;
                    }
                    else if(!strcasecmp(cmdbuffer, "/font"))
                    {
                        token = 1504;
                    }
                    else if(!strcasecmp(cmdbuffer, "/form"))
                    {
                        token = 1505;
                    }
                    else if(!strcasecmp(cmdbuffer, "/span"))
                    {
                        token = 1506;
                    }
                    else if(!strcasecmp(cmdbuffer, "/nobr"))
                    {
                        token = 1507;
                    }
                    else if(!strcasecmp(cmdbuffer, "/code"))
                    {
                        token = 1508;
                    }
                    else if(!strcasecmp(cmdbuffer, "/cite"))
                    {
                        token = 1509;
                    }
                    else if(!strcasecmp(cmdbuffer, "vlink"))
                    {
                        token = 2501;
                    }
                    else if(!strcasecmp(cmdbuffer, "width"))
                    {
                        token = 2502;
                    }
                    else if(!strcasecmp(cmdbuffer, "align"))
                    {
                        token = 2503;
                    }
                    else if(!strcasecmp(cmdbuffer, "class"))
                    {
                        token = 2504;
                    }
                    else if(!strcasecmp(cmdbuffer, "color"))
                    {
                        token = 2505;
                    }
                    else if(!strcasecmp(cmdbuffer, "clear"))
                    {
                        token = 2506;
                    }
                    else if(!strcasecmp(cmdbuffer, "value"))
                    {
                        token = 2507;
                    }
                    else if(!strcasecmp(cmdbuffer, "shape"))
                    {
                        token = 2508;
                    }
                    else if(!strcasecmp(cmdbuffer, "alink"))
                    {
                        token = 2509;
                    }
                    else if(!strcasecmp(cmdbuffer, "white"))
                    {
                        token = 2510;
                    }
                    else if(!strcasecmp(cmdbuffer, "frame"))
                    {
                        token = 2511;
                    }
                }
                break;
            case 6:
                {
                    if (!strcasecmp(cmdbuffer, "script"))
                    {
                        token = 601;
                    }
                    else if(!strcasecmp(cmdbuffer, "center"))
                    {
                        token = 602;
                    }
                    else if(!strcasecmp(cmdbuffer, "strong"))
                    {
                        token = 603;
                    }
                    else if(!strcasecmp(cmdbuffer, "option"))
                    {
                        token = 604;
                    }
                    else if(!strcasecmp(cmdbuffer, "select"))
                    {
                        token = 605;
                    }
                    else if(!strcasecmp(cmdbuffer, "legend"))
                    {
                        token = 606;
                    }
                    else if(!strcasecmp(cmdbuffer, "spacer"))
                    {
                        token = 607;
                    }
                    else if(!strcasecmp(cmdbuffer, "iframe"))
                    {
                        token = 608;
                    }
                    else if(!strcasecmp(cmdbuffer, "object"))
                    {
                        token = 609;
                    }
                    else if(!strcasecmp(cmdbuffer, "/title"))
                    {
                        token = 1601;
                    }
                    else if(!strcasecmp(cmdbuffer, "/style"))
                    {
                        token = 1602;
                    }
                    else if(!strcasecmp(cmdbuffer, "/table"))
                    {
                        token = 1603;
                    }
                    else if(!strcasecmp(cmdbuffer, "/input"))
                    {
                        token = 1604;
                    }
                    else if(!strcasecmp(cmdbuffer, "/blink"))
                    {
                        token = 1605;
                    }
                    else if(!strcasecmp(cmdbuffer, "/small"))
                    {
                        token = 1606;
                    }
                    else if(!strcasecmp(cmdbuffer, "/label"))
                    {
                        token = 1607;
                    }
                    if (!strcasecmp(cmdbuffer, "nowrap"))
                    {
                        token = 2601;
                    }
                    else if(!strcasecmp(cmdbuffer, "valign"))
                    {
                        token = 2602;
                    }
                    else if(!strcasecmp(cmdbuffer, "target"))
                    {
                        token = 2603;
                    }
                    else if(!strcasecmp(cmdbuffer, "height"))
                    {
                        token = 2604;
                    }
                    else if(!strcasecmp(cmdbuffer, "method"))
                    {
                        token = 2605;
                    }
                    else if(!strcasecmp(cmdbuffer, "action"))
                    {
                        token = 2606;
                    }
                    else if(!strcasecmp(cmdbuffer, "onload"))
                    {
                        token = 2607;
                    }
                    else if(!strcasecmp(cmdbuffer, "coords"))
                    {
                        token = 2608;
                    }
                    else if(!strcasecmp(cmdbuffer, "usemap"))
                    {
                        token = 2609;
                    }
                    else if(!strcasecmp(cmdbuffer, "border"))
                    {
                        token = 2610;
                    }
                }
                break;
            case 7:
                {
                    if (!strcasecmp(cmdbuffer, "caption"))
                    {
                        token = 701;
                    }
                    else if(!strcasecmp(cmdbuffer, "marquee"))
                    {
                        token = 702;
                    }
                    else if(!strcasecmp(cmdbuffer, "address"))
                    {
                        token = 703;
                    }
                    else if(!strcasecmp(cmdbuffer, "/script"))
                    {
                        token = 1701;
                    }
                    else if(!strcasecmp(cmdbuffer, "/center"))
                    {
                        token = 1702;
                    }
                    else if(!strcasecmp(cmdbuffer, "/strong"))
                    {
                        token = 1703;
                    }
                    else if(!strcasecmp(cmdbuffer, "/option"))
                    {
                        token = 1704;
                    }
                    else if(!strcasecmp(cmdbuffer, "/select"))
                    {
                        token = 1705;
                    }
                    else if(!strcasecmp(cmdbuffer, "/legend"))
                    {
                        token = 1706;
                    }
                    else if(!strcasecmp(cmdbuffer, "/spacer"))
                    {
                        token = 1707;
                    }
                    else if(!strcasecmp(cmdbuffer, "/iframe"))
                    {
                        token = 1708;
                    }
                    else if(!strcasecmp(cmdbuffer, "/object"))
                    {
                        token = 1709;
                    }
                    else if(!strcasecmp(cmdbuffer, "content"))
                    {
                        token = 2701;
                    }
                    else if(!strcasecmp(cmdbuffer, "bgcolor"))
                    {
                        token = 2702;
                    }
                    else if(!strcasecmp(cmdbuffer, "noshade"))
                    {
                        token = 2703;
                    }
                    else if(!strcasecmp(cmdbuffer, "rowspan"))
                    {
                        token = 2704;
                    }
                    else if(!strcasecmp(cmdbuffer, "colspan"))
                    {
                        token = 2705;
                    }
                    else if(!strcasecmp(cmdbuffer, "checked"))
                    {
                        token = 2706;
                    }
                }
                break;
            case 8:
                {
                    if (!strcasecmp(cmdbuffer, "noscript"))
                    {
                        token = 801;
                    }
                    else if(!strcasecmp(cmdbuffer, "frameset"))
                    {
                        token = 802;
                    }
                    else if(!strcasecmp(cmdbuffer, "noframes"))
                    {
                        token = 803;
                    }
                    else if(!strcasecmp(cmdbuffer, "fieldset"))
                    {
                        token = 804;
                    }
                    else if(!strcasecmp(cmdbuffer, "/caption"))
                    {
                        token = 1801;
                    }
                    else if(!strcasecmp(cmdbuffer, "/marquee"))
                    {
                        token = 1802;
                    }
                    else if(!strcasecmp(cmdbuffer, "/address"))
                    {
                        token = 1803;
                    }
                    else if(!strcasecmp(cmdbuffer, "language"))
                    {
                        token = 2801;
                    }
                    else if(!strcasecmp(cmdbuffer, "collapse"))
                    {
                        token = 2802;
                    }
                    else if(!strcasecmp(cmdbuffer, "noresize"))
                    {
                        token = 2803;
                    }
                    else if(!strcasecmp(cmdbuffer, "selected"))
                    {
                        token = 2804;
                    }
                }
                break;
            case 9:
                {
                    if (!strcasecmp(cmdbuffer, "/noscript"))
                    {
                        token = 1901;
                    }
                    else if(!strcasecmp(cmdbuffer, "/frameset"))
                    {
                        token = 1902;
                    }
                    else if(!strcasecmp(cmdbuffer, "/noframes"))
                    {
                        token = 1903;
                    }
                    else if(!strcasecmp(cmdbuffer, "/fieldset"))
                    {
                        token = 1904;
                    }
                    else if(!strcasecmp(cmdbuffer, "topmargin"))
                    {
                        token = 2901;
                    }
                    else if(!strcasecmp(cmdbuffer, "maxlength"))
                    {
                        token = 2902;
                    }
                    else if(!strcasecmp(cmdbuffer, "helvetica"))
                    {
                        token = 2903;
                    }
                }
                break;
            case 10:
                {
                    if (!strcasecmp(cmdbuffer, "http-equiv"))
                    {
                        token = 3001;
                    }
                    else if(!strcasecmp(cmdbuffer, "leftmargin"))
                    {
                        token = 3002;
                    }
                    else if(!strcasecmp(cmdbuffer, "background"))
                    {
                        token = 3003;
                    }
                    else if(!strcasecmp(cmdbuffer, "onmouseout"))
                    {
                        token = 3004;
                    }
                    else if(!strcasecmp(cmdbuffer, "sans-serif"))
                    {
                        token = 3005;
                    }
                }
                break;
            case 11:
                {
                    if (!strcasecmp(cmdbuffer, "marginwidth"))
                    {
                        token = 3101;
                    }
                    else if(!strcasecmp(cmdbuffer, "cellspacing"))
                    {
                        token = 3102;
                    }
                    else if(!strcasecmp(cmdbuffer, "cellpadding"))
                    {
                        token = 3103;
                    }
                    else if(!strcasecmp(cmdbuffer, "onmouseover"))
                    {
                        token = 3104;
                    }
                    else if(!strcasecmp(cmdbuffer, "bordercolor"))
                    {
                        token = 3105;
                    }
                }
                break;
            case 12:
                {
                    if (!strcasecmp(cmdbuffer, "marginheight"))
                    {
                        token = 3201;
                    }
                    else if(!strcasecmp(cmdbuffer, "bgproperties"))
                    {
                        token = 3202;
                    }
                }
                break;
            case 15:
                {
                    if (!strcasecmp(cmdbuffer, "fprolloverstyle"))
                    {
                        token = 3501;
                    }
                    else if(!strcasecmp(cmdbuffer, "bordercolordark"))
                    {
                        token = 3502;
                    }
                }
                break;
            case 16:
                {
                    if (!strcasecmp(cmdbuffer, "bordercolorlight"))
                    {
                        token = 3601;
                    }
                }
                break;
            }
        }
        break;
    }
    return token;
}

int Chtmlpage::parseCommand(char * htmlcode)
{
    int i;
    int tokenid = 0;
    lowerString(htmlcode);
    i = 0;
    while (htmlcode[i] == '<')
    {
        ++i;
    }
    if (i > 1)
    {
        strcpy(htmlcode + 1, htmlcode + i);
    }
    if (!strncmp(htmlcode, "<link", 5))
    {
        tokenid = 5;
    }
    else if(!strncmp(htmlcode, "<hr", 3))
    {
        tokenid = 6;
    }
    else if(!strncmp(htmlcode, "<base", 5))
    {
        tokenid = 7;
    }
    else if(!strncmp(htmlcode, "<option", 7))
    {
        tokenid = 302;
    }
    else if(!strncmp(htmlcode, "<font", 5))
    {
        tokenid = 303;
    }
    else if(!strncmp(htmlcode, "<center", 7))
    {
        tokenid = 304;
    }
    else if(!strncmp(htmlcode, "<style", 6))
    {
        tokenid = 308;
    }
    else if(!strncmp(htmlcode, "<span", 5))
    {
        tokenid = 309;
    }
    else if(!strncmp(htmlcode, "<iframe", 7))
    {
        tokenid = 321;
    }
    else if(!strncmp(htmlcode, "</base", 6))
    {
        tokenid = 1007;
    }
    else if(!strncmp(htmlcode, "</option", 8))
    {
        tokenid = 1302;
    }
    else if(!strncmp(htmlcode, "</font", 6))
    {
        tokenid = 1303;
    }
    else if(!strncmp(htmlcode, "</center", 8))
    {
        tokenid = 1304;
    }
    else if(!strncmp(htmlcode, "</style", 7))
    {
        tokenid = 1308;
    }
    else if(!strncmp(htmlcode, "</span", 6))
    {
        tokenid = 1309;
    }
    else if(!strncmp(htmlcode, "</iframe", 8))
    {
        tokenid = 1321;
    }
    else if(strlen(htmlcode))
    {
        printf("ParseHTML - unknown htmlcode(%d): #%s#\n", strlen(htmlcode), htmlcode);
    }
    return tokenid;
}

void Chtmlpage::lowerString(char * source)
{
    int i = 0, c;
    while (i >= 0)
    {
        c = source[i];
        switch (c)
        {
        case 0:
            i = - 1;
            break;
        case 'Ö':
            source[i] = 'ö';
            ++i;
            break;
        case 'Ü':
            source[i] = 'ü';
            ++i;
            break;
        case 'Ä':
            source[i] = 'ä';
            ++i;
            break;
        default:
            if ((c >= 'A') && (c <= 'Z'))
            {
                source[i] = c - 'A' + 'a';
            }
            ++i;
            break;
        }
    }
}

void Chtmlpage::mix2Urls(int deep, char * urlmix, char * url1, char * url2)
{
    int tiefe1, tiefe2, p = 0, l;
    tiefe1 = deep;
    tiefe2 = deep;
    switch (url2[0])
    {
    case '/':
    case '\\':
        tiefe1 = 1;
        p = 1;
        break;
    default:
        while (!strncmp(url2 + p, "../", 3))
        {
            tiefe1--;
            p += 3;
        }
        break;
    }
    sprintf(urlmix, "%s", url1);
    if (tiefe1 < 1)
    {
        if (p > 1)
        {
            tiefe1 = 2;
        }
    }
    l = strlen(urlmix);
    while (tiefe2 >= tiefe1)
    {
        l--;
        if (l > 0)
        {
            switch (urlmix[l])
            {
            case '/':
            case '\\':
                {
                    urlmix[l + 1] = 0;
                    tiefe2--;
                }
                break;
            }
        }
        else
        {
            tiefe2 = - tiefe2;
        }
    }
    strcpy(urlmix + l + 1, url2 + p);
}

int Chtmlpage::GetExtPosition(char * zk)
{
    int l, l0, b, b_last = 0, retvalue = 0;
    memset(fileextension, 0, 33);
    l = strlen(zk);
    l0 = l;
    while (l > 0)
    {
        l--;
        b = zk[l];
        switch (b)
        {
        case 10:
        case 13:
        case 32:
            if (!b_last)
            {
                b = 0;
                zk[l] = b;
            }
            break;
        case '.':
            if (!retvalue)
            {
                retvalue = l;
            }
            break;
        case '@':
        case '?':
            l0 = l;
            retvalue = 0;
            break;
        case '/':
        case '\\':
            l = - l;
            break;
        }
        b_last = b;
    }
    l0 -= retvalue;
    if (l0 > 0)
    {
        if (l0 < 32)
        {
            strncpy(fileextension, zk + retvalue, l0);
        }
        else
        {
            strncpy(fileextension, zk + retvalue, 32);
        }
        lowerString(fileextension);
    }
    retvalue += 1000 * l0;
    return retvalue;
}

char * Chtmlpage::GetExtension()
{
    return fileextension;
}

char * Chtmlpage::getFUNKTION()
{
    return GNPparsFUNKTION;
}

char * Chtmlpage::getRESULT()
{
    return GNPreturn;
}

char * Chtmlpage::getSEARCHSTRING()
{
    return GNPparsSEARCH;
}

char * Chtmlpage::getTITLE()
{
    return GNPreturnTITLE;
}

void Chtmlpage::setTMP(char * buffer, int length)
{
    memset(GNPparsTMP, 0, length + 1);
    strncpy(GNPparsTMP, buffer, length);
}

char * Chtmlpage::getTMP()
{
    return GNPparsTMP;
}

char * Chtmlpage::getVARNAME()
{
    return GNPparsVARNAME;
}

void Chtmlpage::clrCOUNTER()
{
    GNPreturnCOUNTER = 0;
}

void Chtmlpage::incCOUNTER()
{
    GNPreturnCOUNTER++;
}

int Chtmlpage::GetNextParameter(const char * parstack)
{
    int parid = 0, i, i2;
    int p;
    int loopmax = 0;
    do
    {
        p = 0;
        if (strlen(parstack))
        {
            sprintf(GNPpars, "%s", parstack);
            strcpy(GNPparsTMP, "");
            strcpy(GNPparsVARNAME, "");
            strcpy(GNPreturn, "");
            strcpy(GNPreturnTITLE, "");
            GNPloopSTART = 0;
            GNPloopEND = strlen(parstack) - 1;
            GNPparsP = GNPloopSTART;
            GNPcount = 0;
        }
        GNPparsSEARCH[0] = 0;
        GNPcount++;
        int parsPold;
        int b0, b1, b2, b3;
        do
        {
            i = GNPparsP;
            parsPold = GNPparsP;
            parid = 0;
            b0 = GNPpars[i];
            if (b0 == ':')
            {
                b1 = GNPpars[i + 1];
                b2 = GNPpars[i + 2];
                b3 = GNPpars[i + 3];
                if (b2 == ':')
                {
                    i++;
                    parid = 0;
                    printf("gnp1:%d(%c)\n", b1, b1);
                    switch (b1)
                    {
                    case 10:
                        GNPparsP++;
                        parid = - 1;
                        break;
                    case 'A':
                        //ok:Anfang
                        GNPloopSTART = i + 1;
                        GNPparsP += 2;
                        parid = - 1;
                        //printf("parCur(%d):set START\n", parid);
                        break;
                    case 'C':
                        //ok:Clear
                        strcpy(GNPreturn, "");
                        strcpy(GNPreturnTITLE, "");
                        strcpy(GNPparsTMP, "");
                        GNPparsP += 2;
                        parid = 901;
                        //printf("parCur(%d):CLEAR\n", parid);
                        break;
                    case 'F':
                        i += 2;
                        i2 = 0;
                        while ((GNPpars[i] != 10) && (GNPpars[i] != ':') && (i <= GNPloopEND))
                        {
                            if (i2 < 128)
                            {
                                GNPparsFUNKTION[i2++] = GNPpars[i++];
                            }
                            else
                            {
                                i++;
                            }
                        }
                        GNPparsFUNKTION[i2] = 0;
                        GNPparsP = i;
                        parid = - 1;
                        break;
                    case 'I':
                        i += 2;
                        i2 = 0;
                        while ((GNPpars[i] != 10) && (GNPpars[i] != ':') && (i <= GNPloopEND))
                        {
                            if (i2 < 128)
                            {
                                GNPparsIF[i2++] = GNPpars[i++];
                            }
                            else
                            {
                                i++;
                            }
                        }
                        GNPparsIF[i2] = 0;
                        if (GNPreturnCOUNTER < atoi(GNPparsIF))
                        {
                            GNPparsP = GNPloopSTART;
                        }
                        else
                        {
                            GNPparsP = i;
                        }
                        parid = - 1;
                        break;
                    case 'E':parid++;
                    case 'S':parid++;
                        getNextPar(GNPparsSEARCH, i + 2);
                        lowerString(GNPparsSEARCH);
                        break;
                    case 'T':
                        parid = 991;
                        getNextPar(GNPparsVARNAME, i + 2);
                        lowerString(GNPparsVARNAME);
                        //printf("parCur(%d):T=%s\n",parid,GNPparsVARNAME);
                        break;
                    case 'V':
                        getNextPar(GNPparsVARNAME, i + 2);
                        lowerString(GNPparsVARNAME);
                        p = strlen(GNPparsVARNAME);
                        //printf("\ngnp> VN=%s GNPtmp=%s p=%d\n", GNPparsVARNAME, GNPparsTMP, p);
                        while (p > 0)
                        {
                            --p;
                            if (GNPparsVARNAME[p] == ';')
                            {
                                GNPparsVARNAME[p] = 0;
                                p = atoi(GNPparsVARNAME + p + 1);
                                if (((unsigned int) p) < strlen(GNPparsTMP))
                                {
                                    int p2;
                                    p2 = strlen(GNPparsTMP);
                                    while (p2 > 0)
                                    {
                                        --p2;
                                        if (GNPparsTMP[p2] == '>')
                                        {
                                            p2 = - (1 + p2);
                                        }
                                    }
                                    if (p2 < 0)
                                    {
                                        p2 = - p2;
                                        strcpy(GNPparsTMP, GNPparsTMP + p2);
                                    }
                                    else
                                    {
                                        GNPparsTMP[p] = 0;
                                    }
                                }
                                else
                                {
                                    //            p = 0;
                                }
                            }
                        }
                        if (p >= 0)
                        {
                            p = strlen(GNPreturn) + strlen(GNPparsTMP);
                            i = atoi(GNPparsVARNAME);
                            sprintf(GNPreturnTITLE + strlen(GNPreturnTITLE), "\"%s\";", GNPparsVARNAME);
                            sprintf(GNPreturn + strlen(GNPreturn), "\"%s\";", GNPparsTMP);
                            if (GNPparsP > 99999)
                            {
                                printf("gnp> varname=%s p=%d t=%s\n", GNPparsVARNAME, p, GNPparsTMP);
                                printf("gnp> I(%d,%d)=%d search=[%s]\n"
                                , GNPreturnCOUNTER, atoi(GNPparsIF), GNPparsP, GNPparsSEARCH);
                                printf("gnp> grt=%s\n", GNPreturnTITLE);
                                printf("gnp> gre=%s\n", GNPreturn);
                            }
                        }
                        else
                        {
                            parid = - 1;
                            GNPparsP = parsPold;
                        }
                        break;
                    case 'X':
                        GNPparsP += 2;
                        parid = 99;
                        //printf("parCur(%d):eXit_and_store\n", parid);
                        break;
                    default:
                        printf("GNP: unknown: %d='%c'\n", GNPpars[i], GNPpars[i]);
                        GNPparsP += 2;
                        break;
                    }
                }
                else if(b3 == ':')
                {
                    parid = 0;
                    printf("gnp2:%d(%c)\n", b1, b1);
                    switch (b1)
                    {
                    case 'E':
                        switch (b2)
                        {
                        case 'C':
                            parid = 21;
                            getNextPar(GNPparsSEARCH, i + 4);
                            lowerString(GNPparsSEARCH);
                            break;
                        default:
                            GNPparsP += 3;
                            break;
                        }
                        break;
                    case 'N':
                        switch (b2)
                        {
                        case 'T':
                            parid = 101;
                            getNextPar(GNPparsVARNAME, i + 4);
                            break;
                        default:
                            GNPparsP += 3;
                            break;
                        }
                        break;
                    case 'S':
                        switch (b2)
                        {
                        case 'C':
                            parid = 22;
                            getNextPar(GNPparsSEARCH, i + 4);
                            lowerString(GNPparsSEARCH);
                            //printf("parCur(%d):SC=%s\n",parid,GNPparsSEARCH);
                            break;
                        case 'T':
                            parid = 23;
                            getNextPar(GNPparsSEARCH, i + 4);
                            break;
                        default:
                            GNPparsP += 3;
                            break;
                        }
                        break;
                    case 'U':
                        switch (b2)
                        {
                        case 'T':
                            //ok
                            parid = 11;
                            getNextPar(GNPparsSEARCH, i + 4);
                            lowerString(GNPparsSEARCH);
                            //printf("parCur(%d):UT=%s\n", parid, GNPparsSEARCH);
                            break;
                        default:
                            GNPparsP += 3;
                            break;
                        }
                        break;
                    default:
                        GNPparsP += 3;
                        break;
                    }
                }
                else
                {
                    GNPparsP++;
                }
            }
            else
            {
                GNPparsP++;
            }
            if (GNPparsP > GNPloopEND)
            {
                GNPparsP = GNPloopSTART;
            }
        }
        while (parid < 0);
        if (parid)
        {
            loopmax = 10;
        }
        else
        {
            loopmax++;
        }
    }
    while (loopmax < 10);
    printf("gnp_end\n");
    return parid;
}

void Chtmlpage::getNextPar(char * target, int startpos)
{
    int i, i2 = 0;
    i = startpos;
    do
    {
        switch (GNPpars[i])
        {
        case 10:
        case ':':
            GNPloopEND = - GNPloopEND;
            break;
        default:
            target[i2++] = GNPpars[i++];
            break;
        }
    }
    while (i <= GNPloopEND);
    if (GNPloopEND < 0)
    {
        GNPloopEND = - GNPloopEND;
    }
    target[i2] = 0;
    if (i <= GNPloopEND)
    {
        GNPparsP = i;
    }
    else
    {
        GNPparsP = 0;
    }
}

int Chtmlpage::GetFiletype(char * filename)
{
    char * erg = NULL;
    filetype = 1;
    subtype = 0;
    (void) GetExtPosition(filename);
    if (fileextension[0])
    {
        erg = strstr(".wd3;05;00.dup;91;00.bak;92;00.ism;93;00.part;08;00.met;09;00.bak;01;00"
        , fileextension);
        if (!erg)
        {
            erg = strstr(".jpe;07;01.jpeg;07;01.jpg;07;01.png;07;02.gif;07;03.bmp;07;11.tif;07;12.pcx;07;13"
            , fileextension);
        }
        if (!erg)
        {
            erg = strstr(".asf;06;00.asx;06;00.swf;06;00.avi;06;01.mpg;06;02.mpeg;06;02.wmv;06;03.wmf;06;04"
            , fileextension);
        }
        if (!erg)
        {
            erg = strstr(".htm;04;01.html;04;01.shtm;04;01.shtml;04;01.primary;04;01.php;04;02.php3;04;02"
            , fileextension);
        }
        if (!erg)
        {
            erg = strstr(".asp;04;03.cfm;04;04.cgi;04;05.txt;04;99"
            , fileextension);
        }
        if (!erg)
        {
            erg = strstr(".dkt;11;00.cleanup;12;00"
            , fileextension);
        }
        if (erg)
        {
            filetype = atoi(erg + strlen(fileextension) + 1);
            subtype = atoi(erg + strlen(fileextension) + 4);
        }
        //printf("ft(%d,%d:%s)=%s\n", filetype, subtype, filename, fileextension);
    }
    else
    {
        filetype = 4;
        subtype = 99;
    }
    return filetype;
}

int Chtmlpage::GetFilesubtype()
{
    return subtype;
}

char * Chtmlpage::extractFilename(char * source)
{
    int vokal, start = 0;
    int f, z, b, i, l, ifn;
    unsigned int zcount = 0, pcount = 0;
    char tmp[strlen(source) + 1];
    strcpy(tmp, source);
    do
    {
        source = tmp;
        l = strlen(tmp);
        i = l;
        start = 0;
        vokal = 0;
        while (i > 0)
        {
            i--;
            switch (tmp[i])
            {
            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'i':
            case 'I':
            case 'o':
            case 'O':
            case 'u':
            case 'U':
            case 'y':
            case 'Y':
            case 'ä':
            case 'Ä':
            case 'ö':
            case 'Ö':
            case 'ü':
            case 'Ü':
                vokal++;
                break;
            case '/':
            case '\\':
                tmp[i] = 0;
                start = i + 1;
                source = tmp + start;
                i = - (i + 1);
                break;
            }
        }
        if (i < 0)
        {
            if (!vokal)
            {
                i = 99;
            }
            else if(!strncasecmp(source, "thumb", 5))
            {
                i = 99;
            }
            else if(!strncasecmp(source, "index", 5))
            {
                i = 99;
            }
            else if(!strncasecmp(source, "free", 4))
            {
                i = 99;
            }
            else if(!strncasecmp(source, "gall", 4))
            {
                i = 99;
            }
            else if(!strncasecmp(source, "movi", 4))
            {
                i = 99;
            }
            else if(!strncasecmp(source, "pic", 3))
            {
                i = 99;
            }
            else
            {
                //printf("eF(%d):%s\n", l, source);
            }
        }
    }
    while (i > 0);
    l -= start;
    i = 0;
    ifn = 0;
    z = 0;
    f = 0;
    while (i <= l)
    {
        if (i == l)
        {
            b = '_';
            i++;
        }
        else
        {
            b = source[i++];
        }
        if ((b >= 'a') && (b <= 'z'))
        {
            if (z)
            {
                targetfilename[ifn++] = b;
                zcount++;
            }
            else
            {
                b += 'A' - 'a';
                f = 1;
            }
            z = 1;
        }
        else if((b >= 'A') && (b <= 'Z'))
        {
            if (z == 1)
            {
                z = 0;
            }
            if (z)
            {
                targetfilename[ifn++] = b - 'A' + 'a';
                zcount++;
            }
            else
            {
                f = 1;
            }
            targetfilename[ifn] = 0;
            z = 2;
        }
        else if((b >= '0') && (b <= '9'))
        {
            i = l + 1;
            f = 2;
            z = 0;
        }
        else
        {
            switch (b)
            {
            case 'ö':
            case 'ä':
            case 'ü':
            case 'ß':
                targetfilename[ifn++] = b;
                zcount++;
                z = 1;
                break;
            case 'Ö':
            case 'Ä':
            case 'Ü':
                z = 1;
                targetfilename[ifn++] = b;
                zcount++;
                break;
            case ' ':
            case '_':
            case '-':
                z = 0;
                f = 2;
                break;
            case '.':
                zcount = 0;
                i = l + 1;
                break;
            default:
                break;
            }
        }
        switch (f)
        {
        case 1:
        case 2:
            {
                if (pcount < 4)
                {
                    if (zcount)
                    {
                        pcount++;
                        if (ifn)
                        {
                            int p = 0;
                            while (p < exblockAnz)
                            {
                                p++;
                                if (strlen(exblock[p]) == zcount)
                                {
                                    if (!strncmp(targetfilename + ifn - zcount, exblock[p], zcount))
                                    {
                                        ifn -= zcount;
                                        zcount = 0;
                                        p = exblockAnz;
                                        pcount--;
                                    }
                                }
                            }
                            if (zcount)
                            {
                                targetfilename[ifn++] = '_';
                                zcount = 0;
                            }
                        }
                    }
                    if (f == 1)
                    {
                        targetfilename[ifn++] = b;
                        zcount = 1;
                    }
                    f = 0;
                }
                else
                {
                    i = l + 1;
                }
            }
            break;
        }
    }
    while (ifn > 0)
    {
        if (targetfilename[ifn - 1] == '_')
        {
            ifn--;
        }
        else
        {
            ifn = - ifn;
        }
    }
    if (ifn < 0)
    {
        ifn = - ifn;
    }
    targetfilename[ifn++] = 0;
    return targetfilename;
}

