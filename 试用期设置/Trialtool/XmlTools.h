/*
	使用静态库方式编译不过，因此先采用包含类的方式
*/
#ifndef XMLTOOLS_H
#define XMLTOOLS_H


#include "./pugixml/src/pugixml.hpp"
#include <string>
#include <vector>
#include <map>


struct XmlTraverseParams
{
	// 节点是否是有值类型的节点，
	// 如果没有值，则名称不为空，并且可以有属性，
	// 如果有值，则名称为空，属性为空
	bool hasValue;
	
	// 节点名称
	std::string node_Name;

	// 节点值
	std::string node_Value;

	// 节点属性列表
	std::vector<std::pair<std::string, std::string>> vec_pair_Attributes;
};

//遍历节点时传出的参数
typedef void (*TraverseFun)(XmlTraverseParams *nodeParams);

/**
 * @ClassName: 		XmlTools
 * @Description:	基于pugi库的xml读写配置文件类
 *					提供属性和值两种存储配置信息的接口
 *					属性的形式：<DataBaseConnect driver="SQL SERVER" version="SqlServer2008"></DataBaseConnect>
 *					值的形式：<ServerIP>127.0.0.1</ServerIP>
 * @Author:			
 * @Date:			2018/11/12
 */
class XmlTools
{
public:
	//************************************
	// Method:    GetInstance				单例模式
	// FullName:  XmlTools::GetInstance
	// Access:    public static 
	// Returns:   XmlTools *				返回单例指针
	// Qualifier:
	//************************************
	static XmlTools *GetInstance();
private:
	XmlTools(void);
public:
	~XmlTools(void);

public:
	//************************************
	// Method:    LoadXmlFile				读取xml文件
	// FullName:  XmlTools::LoadXmlFile
	// Access:    public 
	// Returns:   bool						读取成功返回true，其余xml格式不正确等情况返回false
	// Qualifier:
	// Parameter: const char * filePath		文件完整路径名
	//************************************
	bool LoadXmlFile(const char *filePath);

	//************************************
	// Method:    IsLoaded					xml是否成功读取
	// FullName:  XmlTools::IsLoaded
	// Access:    public 
	// Returns:   bool						成功返回true， 否则返回false
	// Qualifier: const
	//************************************
	bool IsLoaded() const;

	//************************************
	// Method:    GetXmlFilePath			获取当前读取的xml文件完整路径
	// FullName:  XmlTools::GetXmlFilePath
	// Access:    public 
	// Returns:   const char *				返回文件完整路径
	// Qualifier: const
	//************************************
	const char *GetXmlFilePath() const;

	//************************************
	// Method:    GetValue					获取节点值
	// FullName:  XmlTools::GetValue
	// Access:    public 
	// Returns:   const char *				返回节点的值，如节点形式为<ServerIP>127.0.0.1</ServerIP>节点值为127.0.0.1
	// Qualifier:
	// Parameter: const char * nodeXPath	节点的完整树型路径，如"DataBaseConnect/ServerIP"
	// Parameter: const char * delims		节点路径的分割符，默认为"/"
	//************************************
	const char *GetValue(const char *nodeXPath, const char *delims = "/");

	//************************************
	// Method:    SetValue					设置节点值
	// FullName:  XmlTools::SetValue
	// Access:    public 
	// Returns:   bool						设置成功返回true，否则返回false
	// Qualifier:
	// Parameter: const char * nodeXPath	节点的完整树型路径，如"DataBaseConnect/ServerIP"
	// Parameter: const char * valueStr		节点的值，字符串形式
	// Parameter: const char * delims		节点路径的分割符，默认为"/"
	//************************************
	bool SetValue(const char *nodeXPath, const char *valueStr, const char *delims = "/");

	//************************************
	// Method:    GetAttribute				获取节点属性
	// FullName:  XmlTools::GetAttribute
	// Access:    public 
	// Returns:   const char *				节点的属性，如<DataBaseConnect driver="SQL SERVER" version="SqlServer2008"></DataBaseConnect>
	//										节点DataBaseConnect附加的属性driver值为"SQL SERVER"
	// Qualifier:
	// Parameter: const char * nodeXPath	 节点的完整树型路径，如"DataBaseConnect/ServerIP"
	// Parameter: const char * nodeAttribute 节点的属性名，字符串形式，如"driver"
	// Parameter: const char * delims		 节点路径的分割符，默认为"/"
	//************************************
	const char *GetAttribute(const char *nodeXPath, const char *nodeAttribute, const char *delims = "/");

	//************************************
	// Method:    SetAttribute				 设置节点属性
	// FullName:  XmlTools::SetAttribute
	// Access:    public 
	// Returns:   bool						 设置成功返回true， 否则返回false
	// Qualifier:
	// Parameter: const char * nodeXPath	 节点的完整树型路径，如"DataBaseConnect/ServerIP"
	// Parameter: const char * nodeAttribute 节点的属性名，字符串形式，如"driver"
	// Parameter: const char * valueStr		 节点的属性值，字符串形式，如"SQL SERVER"
	// Parameter: const char * delims		 节点路径的分割符，默认为"/"
	//************************************
	bool SetAttribute(const char *nodeXPath, const char *nodeAttribute, const char *valueStr, const char *delims = "/");

	//************************************
	// Method:    SplitString				字符串分割
	// FullName:  XmlTools::SplitString
	// Access:    public static 
	// Returns:   std::vector<std::string>	返回分割后的字符串数组，格式为string
	// Qualifier:
	// Parameter: const char * src			待分割的源字符串
	// Parameter: const char * delims		分割符，默认为"/"
	//************************************
	static std::vector<std::string> SplitString(const char *src, const char *delims = "/");

	// 通过XPath遍历节点，接口函数自定义
	void TraverseXmlNode(const char *nodeXPath, TraverseFun customfun = NULL, const char *delims = "/");

	//************************************
	// Method:    testXml					测试函数
	// FullName:  XmlTools::testXml
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	void testXml();

private:
	//************************************
	// Method:    CompleteNodesOnXPath		设置值或属性的时候补全路径上的节点
	// FullName:  XmlTools::CompleteNodesOnXPath
	// Access:    private 
	// Returns:   bool						成功返回true，否则返回false
	// Qualifier:
	// Parameter: const char * nodeXPath	节点路径，如"DataBaseConnect/ServerIP"，
	//										依次判断DataBaseConnect、ServerIP，如果没有则补全
	// Parameter: const char * delims		节点路径的分割符，默认为"/"
	//************************************
	bool CompleteNodesOnXPath(const char *nodeXPath, const char *delims = "/");

	//************************************
	// Method:    PrintNodeInfo				打印节点信息，类型、名称、值、所有属性
	// FullName:  XmlTools::PrintNodeInfo
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: pugi::xml_node & node
	//************************************
	bool PrintNodeInfo(pugi::xml_node& node);

	//************************************
	// Method:    PrintAttributeInfo		打印属性信息，属性名，属性值
	// FullName:  XmlTools::PrintAttributeInfo
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: pugi::xml_attribute & xmlAttribute
	//************************************
	bool PrintAttributeInfo(pugi::xml_attribute &xmlAttribute);

	//************************************
	// Method:    TraverseXmlNode			递归遍历节点
	// FullName:  XmlTools::TraverseRoot
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: pugi::xml_node & node
	//************************************
	void TraverseRoot(pugi::xml_node &node, TraverseFun customfun = NULL); 
	
	//************************************
	// Method:    TraverseAttributes		顺序遍历节点的所有属性
	// FullName:  XmlTools::TraverseAttributes
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: pugi::xml_node & node
	//************************************
	void TraverseAttributes(pugi::xml_node &node);

private:
	// 当前读取的xml文件完整路径
	std::string m_xmlFilePath;

	// 是否读取成功
	bool m_Loaded;

	// xml根节点
	pugi::xml_document m_document;
};

#endif	// XMLTOOLS_H
