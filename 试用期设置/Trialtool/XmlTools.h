/*
	ʹ�þ�̬�ⷽʽ���벻��������Ȳ��ð�����ķ�ʽ
*/
#ifndef XMLTOOLS_H
#define XMLTOOLS_H


#include "./pugixml/src/pugixml.hpp"
#include <string>
#include <vector>
#include <map>


struct XmlTraverseParams
{
	// �ڵ��Ƿ�����ֵ���͵Ľڵ㣬
	// ���û��ֵ�������Ʋ�Ϊ�գ����ҿ��������ԣ�
	// �����ֵ��������Ϊ�գ�����Ϊ��
	bool hasValue;
	
	// �ڵ�����
	std::string node_Name;

	// �ڵ�ֵ
	std::string node_Value;

	// �ڵ������б�
	std::vector<std::pair<std::string, std::string>> vec_pair_Attributes;
};

//�����ڵ�ʱ�����Ĳ���
typedef void (*TraverseFun)(XmlTraverseParams *nodeParams);

/**
 * @ClassName: 		XmlTools
 * @Description:	����pugi���xml��д�����ļ���
 *					�ṩ���Ժ�ֵ���ִ洢������Ϣ�Ľӿ�
 *					���Ե���ʽ��<DataBaseConnect driver="SQL SERVER" version="SqlServer2008"></DataBaseConnect>
 *					ֵ����ʽ��<ServerIP>127.0.0.1</ServerIP>
 * @Author:			
 * @Date:			2018/11/12
 */
class XmlTools
{
public:
	//************************************
	// Method:    GetInstance				����ģʽ
	// FullName:  XmlTools::GetInstance
	// Access:    public static 
	// Returns:   XmlTools *				���ص���ָ��
	// Qualifier:
	//************************************
	static XmlTools *GetInstance();
private:
	XmlTools(void);
public:
	~XmlTools(void);

public:
	//************************************
	// Method:    LoadXmlFile				��ȡxml�ļ�
	// FullName:  XmlTools::LoadXmlFile
	// Access:    public 
	// Returns:   bool						��ȡ�ɹ�����true������xml��ʽ����ȷ���������false
	// Qualifier:
	// Parameter: const char * filePath		�ļ�����·����
	//************************************
	bool LoadXmlFile(const char *filePath);

	//************************************
	// Method:    IsLoaded					xml�Ƿ�ɹ���ȡ
	// FullName:  XmlTools::IsLoaded
	// Access:    public 
	// Returns:   bool						�ɹ�����true�� ���򷵻�false
	// Qualifier: const
	//************************************
	bool IsLoaded() const;

	//************************************
	// Method:    GetXmlFilePath			��ȡ��ǰ��ȡ��xml�ļ�����·��
	// FullName:  XmlTools::GetXmlFilePath
	// Access:    public 
	// Returns:   const char *				�����ļ�����·��
	// Qualifier: const
	//************************************
	const char *GetXmlFilePath() const;

	//************************************
	// Method:    GetValue					��ȡ�ڵ�ֵ
	// FullName:  XmlTools::GetValue
	// Access:    public 
	// Returns:   const char *				���ؽڵ��ֵ����ڵ���ʽΪ<ServerIP>127.0.0.1</ServerIP>�ڵ�ֵΪ127.0.0.1
	// Qualifier:
	// Parameter: const char * nodeXPath	�ڵ����������·������"DataBaseConnect/ServerIP"
	// Parameter: const char * delims		�ڵ�·���ķָ����Ĭ��Ϊ"/"
	//************************************
	const char *GetValue(const char *nodeXPath, const char *delims = "/");

	//************************************
	// Method:    SetValue					���ýڵ�ֵ
	// FullName:  XmlTools::SetValue
	// Access:    public 
	// Returns:   bool						���óɹ�����true�����򷵻�false
	// Qualifier:
	// Parameter: const char * nodeXPath	�ڵ����������·������"DataBaseConnect/ServerIP"
	// Parameter: const char * valueStr		�ڵ��ֵ���ַ�����ʽ
	// Parameter: const char * delims		�ڵ�·���ķָ����Ĭ��Ϊ"/"
	//************************************
	bool SetValue(const char *nodeXPath, const char *valueStr, const char *delims = "/");

	//************************************
	// Method:    GetAttribute				��ȡ�ڵ�����
	// FullName:  XmlTools::GetAttribute
	// Access:    public 
	// Returns:   const char *				�ڵ�����ԣ���<DataBaseConnect driver="SQL SERVER" version="SqlServer2008"></DataBaseConnect>
	//										�ڵ�DataBaseConnect���ӵ�����driverֵΪ"SQL SERVER"
	// Qualifier:
	// Parameter: const char * nodeXPath	 �ڵ����������·������"DataBaseConnect/ServerIP"
	// Parameter: const char * nodeAttribute �ڵ�����������ַ�����ʽ����"driver"
	// Parameter: const char * delims		 �ڵ�·���ķָ����Ĭ��Ϊ"/"
	//************************************
	const char *GetAttribute(const char *nodeXPath, const char *nodeAttribute, const char *delims = "/");

	//************************************
	// Method:    SetAttribute				 ���ýڵ�����
	// FullName:  XmlTools::SetAttribute
	// Access:    public 
	// Returns:   bool						 ���óɹ�����true�� ���򷵻�false
	// Qualifier:
	// Parameter: const char * nodeXPath	 �ڵ����������·������"DataBaseConnect/ServerIP"
	// Parameter: const char * nodeAttribute �ڵ�����������ַ�����ʽ����"driver"
	// Parameter: const char * valueStr		 �ڵ������ֵ���ַ�����ʽ����"SQL SERVER"
	// Parameter: const char * delims		 �ڵ�·���ķָ����Ĭ��Ϊ"/"
	//************************************
	bool SetAttribute(const char *nodeXPath, const char *nodeAttribute, const char *valueStr, const char *delims = "/");

	//************************************
	// Method:    SplitString				�ַ����ָ�
	// FullName:  XmlTools::SplitString
	// Access:    public static 
	// Returns:   std::vector<std::string>	���طָ����ַ������飬��ʽΪstring
	// Qualifier:
	// Parameter: const char * src			���ָ��Դ�ַ���
	// Parameter: const char * delims		�ָ����Ĭ��Ϊ"/"
	//************************************
	static std::vector<std::string> SplitString(const char *src, const char *delims = "/");

	// ͨ��XPath�����ڵ㣬�ӿں����Զ���
	void TraverseXmlNode(const char *nodeXPath, TraverseFun customfun = NULL, const char *delims = "/");

	//************************************
	// Method:    testXml					���Ժ���
	// FullName:  XmlTools::testXml
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	void testXml();

private:
	//************************************
	// Method:    CompleteNodesOnXPath		����ֵ�����Ե�ʱ��ȫ·���ϵĽڵ�
	// FullName:  XmlTools::CompleteNodesOnXPath
	// Access:    private 
	// Returns:   bool						�ɹ�����true�����򷵻�false
	// Qualifier:
	// Parameter: const char * nodeXPath	�ڵ�·������"DataBaseConnect/ServerIP"��
	//										�����ж�DataBaseConnect��ServerIP�����û����ȫ
	// Parameter: const char * delims		�ڵ�·���ķָ����Ĭ��Ϊ"/"
	//************************************
	bool CompleteNodesOnXPath(const char *nodeXPath, const char *delims = "/");

	//************************************
	// Method:    PrintNodeInfo				��ӡ�ڵ���Ϣ�����͡����ơ�ֵ����������
	// FullName:  XmlTools::PrintNodeInfo
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: pugi::xml_node & node
	//************************************
	bool PrintNodeInfo(pugi::xml_node& node);

	//************************************
	// Method:    PrintAttributeInfo		��ӡ������Ϣ��������������ֵ
	// FullName:  XmlTools::PrintAttributeInfo
	// Access:    private 
	// Returns:   bool
	// Qualifier:
	// Parameter: pugi::xml_attribute & xmlAttribute
	//************************************
	bool PrintAttributeInfo(pugi::xml_attribute &xmlAttribute);

	//************************************
	// Method:    TraverseXmlNode			�ݹ�����ڵ�
	// FullName:  XmlTools::TraverseRoot
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: pugi::xml_node & node
	//************************************
	void TraverseRoot(pugi::xml_node &node, TraverseFun customfun = NULL); 
	
	//************************************
	// Method:    TraverseAttributes		˳������ڵ����������
	// FullName:  XmlTools::TraverseAttributes
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: pugi::xml_node & node
	//************************************
	void TraverseAttributes(pugi::xml_node &node);

private:
	// ��ǰ��ȡ��xml�ļ�����·��
	std::string m_xmlFilePath;

	// �Ƿ��ȡ�ɹ�
	bool m_Loaded;

	// xml���ڵ�
	pugi::xml_document m_document;
};

#endif	// XMLTOOLS_H
