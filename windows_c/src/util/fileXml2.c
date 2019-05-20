#include <stdio.h>
//#include <string.h>
#include <libxml/xmlreader.h>
#include <time.h>


static FILE *file;
void printAttribute(xmlTextReaderPtr reader)
{
    if(1==xmlTextReaderHasAttributes(reader))
    {
        xmlChar *name,*value;
        int res=xmlTextReaderMoveToFirstAttribute(reader);//首先移动current instance 指向第一个元素
        while(1==res)
        {
            name=xmlTextReaderConstName(reader);
            value=xmlTextReaderConstValue(reader);
            fprintf(file,"\tattribute=[%s],value=[%s]\n",name,value);
            res=xmlTextReaderMoveToNextAttribute(reader);
        }
        xmlTextReaderMoveToElement(reader);//重新将current instance指向current node, 否则之后读取节点value时会出现混乱
    }
}
void printNode(xmlTextReaderPtr reader)
{
    const xmlChar *name,*value;
    name=xmlTextReaderConstName(reader);

    if(xmlTextReaderNodeType(reader)==XML_READER_TYPE_ELEMENT)
    {
        fprintf(file,"startMarkup=%s",name);
        printAttribute(reader);
    }else if(xmlTextReaderNodeType(reader)==XML_READER_TYPE_END_ELEMENT)
    {
        //fprintf(file,"endMarkup=%s",name);
    }else
    {
        fprintf(file,"flag=%s",name);
    }
    if(xmlTextReaderHasValue(reader))
    {
        value=xmlTextReaderConstValue(reader);
        fprintf(file,"\tvalue=%s",value);
    }
    if(strcmp(name,"html")==0){
    	printf("读取到html标签！");
    }

    fprintf(file,"\n");
}

int main8()
{
//    LIBXML_TEST_VERSION
//    char *buffer="<root>"
//    "<student>123</student>"
//    "</root>";
	char *buffer=getBuff();

    xmlTextReaderPtr reader;
//  reader=xmlReaderForFile(buffer,NULL,XML_PARSE_NOBLANKS);//忽略空节点
    reader=xmlReaderForMemory(buffer,strlen(buffer),"",NULL,0);

    if(reader==NULL)
    {
        fprintf(stderr,"error to read");
        xmlCleanupParser();
        return(1);
    }
    file=fopen("d:/result.xml","w");

    int ret=xmlTextReaderRead(reader);
    while(ret==1)
    {
        printNode(reader);
        ret=xmlTextReaderRead(reader);
    }

    fclose(file);
    xmlFreeTextReader(reader);
    if(ret!=0)
    {
        fprintf(stderr,"error to read");
        xmlCleanupParser();
        return(1);
    }

    xmlCleanupParser();

    return(0);
}
