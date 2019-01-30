#include <stdio.h>
#include <string.h>
#define IN_LIBXML
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
//在mingw环境下，xmlfree 等可能出现问题，见http://blog.csdn.net/king_on/article/details/7543577
#define IN_LIBXML

//#include <libxml/parser.h>
#include <libxml/tree.h>

/**
判断节点是否是叶子节点
1. 如果node=NULL，return 0
2. 如果node->type不是XML_ELEMENT_NODE，return 0
3. 如果node->type为XML_ELEMENT_NODE,并且node->children中没有XML_ELEMENT_NODE类型的节点，那么return 1
*/
int isLeafNode(xmlNode *node)
{
    if(node==NULL)
        return(0);

    if(node->type!=XML_ELEMENT_NODE)
        return(0);

    xmlNode *childNode;
    for(childNode=node->children;childNode!=NULL;childNode=childNode->next)
    {
        if(childNode->type==XML_ELEMENT_NODE)
            return(0);
    }
    return(1);
}
void print(xmlDocPtr doc,xmlNode *rootNode)
{
    xmlNode *curNode;
    xmlChar *v;
    for(curNode=rootNode; curNode!=NULL; curNode=curNode->next)
    {
        if(curNode->type==XML_ELEMENT_NODE)
        {
            printf("node name:%s",curNode->name);
            if(isLeafNode(curNode))
            {
                v=xmlNodeGetContent(curNode);//获取节点内容
                printf("\tnode value:%s",v);
                xmlFree(v);//施放v
            }
            xmlAttr *attr=curNode->properties;//首个节点或NULL
            while(attr!=NULL)
            {
                v=xmlGetProp(curNode,attr->name);//获取属性内容
                printf("\tattr(%s)=%s",attr->name,v);
                xmlFree(v);//施放v
                attr=attr->next;
            }
            printf("\n");
        }
        print(doc,curNode->children);
    }
}

int main6(int argv,char *argc[])
{
    LIBXML_TEST_VERSION

    xmlDocPtr doc;
    //XML_PARSE_NOBLANKS 在解析时忽略空节点
    doc=xmlReadFile("d:/test1.xml",NULL,XML_PARSE_NOBLANKS);

    //在mingw环境下，xmlfree 等可能出现问题，见http://blog.csdn.net/king_on/article/details/7543577
    if(!xmlFree) xmlMemGet(&xmlFree,&xmlMalloc,&xmlRealloc,NULL);

    xmlNode *rootNode=xmlDocGetRootElement(doc);
    print(doc,rootNode);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return(0);
}
