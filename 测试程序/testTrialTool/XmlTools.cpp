#include "XmlTools.h"

#include <Windows.h>

XmlTools::XmlTools(void)
	: m_Loaded(false), m_xmlFilePath("")
{
}

XmlTools::~XmlTools(void)
{
}

XmlTools * XmlTools::GetInstance()
{
	static XmlTools xmltool;
	return &xmltool;
}

bool XmlTools::LoadXmlFile( const char *filePath )
{
	pugi::xml_parse_result result = m_document.load_file(filePath, pugi::parse_full);
	if (pugi::status_ok != result.status)
	{
		OutputDebugStringA(result.description());
		m_Loaded = false;
		return false;
	}

	m_Loaded = true;

	m_xmlFilePath = filePath;

	//PrintNodeInfo(m_document.last_child());

	return true;
}

bool XmlTools::IsLoaded() const
{
	return m_Loaded;
}

const char * XmlTools::GetXmlFilePath() const
{
	return m_xmlFilePath.c_str();
}

std::vector<std::string> XmlTools::SplitString( const char *src, const char *delims /*= "/"*/ )
{
	std::vector<std::string> vecRet;

	std::string srcString(src);

	std::string::size_type startPos = 0;
	std::string::size_type curPos = startPos;
	std::string strTocken;
	while(true)
	{
		startPos = curPos;
		curPos = srcString.find_first_of(delims, curPos);
		strTocken = srcString.substr(startPos, curPos - startPos);
		vecRet.push_back(strTocken);

		if (std::string::npos == curPos)
		{
			break;
		}
		else
		{
			curPos += ::strlen(delims);
		}
	}

	return vecRet;
}

void XmlTools::TraverseAttributes( pugi::xml_node &node )
{
	if (node.empty())
	{
		return;
	}
	else
	{
		for (auto itr = node.attributes_begin(); itr != node.attributes_end(); ++itr)
		{
			PrintAttributeInfo(*itr);
		}
	}
}

void XmlTools::TraverseXmlNode( const char *nodeXPath, TraverseFun customfun, const char *delims )
{
	if (!m_Loaded || ::strlen(nodeXPath) == 0)
	{
		return;
	}

	pugi::xml_node node = m_document.first_element_by_path(nodeXPath);

	TraverseRoot(node, customfun);
}

void XmlTools::TraverseRoot( pugi::xml_node &node, TraverseFun customfun )
{
	if (!node.empty())
	{
		//PrintNodeInfo(node);
		if (NULL != customfun)
		{
			XmlTraverseParams *pXmlTraverseParams = new XmlTraverseParams();
			pXmlTraverseParams->node_Name = node.name();
			pXmlTraverseParams->node_Value = node.value();
			if (pugi::node_pcdata == node.type())
			{
				pXmlTraverseParams->hasValue = true;
			}
			else
			{
				pXmlTraverseParams->hasValue = false;
			}

			for (auto itr = node.attributes_begin(); itr != node.attributes_end(); ++itr)
			{
				if (!itr->empty())
				{
					pXmlTraverseParams->vec_pair_Attributes.push_back(std::make_pair(itr->name(), itr->value()));
				}
			}

			customfun(pXmlTraverseParams);
		}
		
		for (pugi::xml_node::iterator itr = node.begin(); itr != node.end(); ++itr)
		{
			TraverseRoot(*itr, customfun);
		}
	}
}

bool XmlTools::PrintNodeInfo( pugi::xml_node& node )
{
	if (node.empty())
	{
		return false;
	}
	else
	{
		char dbg[1024] = {0};
		sprintf_s(dbg, "PrintNodeInfo: type = %d, name = %s, value = %s \n", node.type(), node.name(), node.value());
		::OutputDebugStringA(dbg);

		TraverseAttributes(node);
		return true;
	}
}

bool XmlTools::PrintAttributeInfo( pugi::xml_attribute &xmlAttribute )
{
	if (xmlAttribute.empty())
	{
		return false;
	}
	else
	{
		char dbg[1024] = {0};
		sprintf_s(dbg, "PrintAttributeInfo: %s = ""%s"" \n", xmlAttribute.name(), xmlAttribute.value());
		::OutputDebugStringA(dbg);
		return true;
	}

}

bool XmlTools::CompleteNodesOnXPath( const char *nodeXPath, const char *delims /*= "/"*/ )
{
	if (!m_Loaded || ::strlen(nodeXPath) == 0)
	{
		return false;
	}

	auto vecXPath = SplitString(nodeXPath, delims);

	pugi::xml_node xmlNode = m_document;

	for (auto itr = vecXPath.begin(); itr != vecXPath.end(); ++itr)
	{
		if (!itr->empty())
		{
			if (xmlNode.child(itr->c_str()).empty())
			{
				xmlNode.append_child(itr->c_str());
			}
			xmlNode = xmlNode.child(itr->c_str());
		}
	}

	return true;
}

const char * XmlTools::GetValue( const char *nodeXPath, const char *delims /*= "/"*/ )
{
	if (!m_Loaded || ::strlen(nodeXPath) == 0)
	{
		return "";
	}

	return m_document.first_element_by_path(nodeXPath).first_child().value();
}

bool XmlTools::SetValue( const char *nodeXPath, const char *valueStr, const char *delims /*= "/"*/ )
{
	if (!m_Loaded || ::strlen(nodeXPath) == 0)
	{
		return false;
	}

	if (m_document.first_element_by_path(nodeXPath).empty())
	{
		CompleteNodesOnXPath(nodeXPath, delims);
	}

	if (m_document.first_element_by_path(nodeXPath).first_child().empty())
	{
		m_document.first_element_by_path(nodeXPath).append_child(pugi::node_pcdata);
	}

	m_document.first_element_by_path(nodeXPath).first_child().set_value(valueStr);

	m_document.save_file(m_xmlFilePath.c_str());

	return true;
}

const char * XmlTools::GetAttribute( const char *nodeXPath, const char *nodeAttribute, const char *delims /*= "/"*/ )
{
	if (!m_Loaded || ::strlen(nodeXPath) == 0)
	{
		return "";
	}

	return m_document.first_element_by_path(nodeXPath).attribute(nodeAttribute).value();
}

bool XmlTools::SetAttribute( const char *nodeXPath, const char *nodeAttribute, const char *valueStr, const char *delims /*= "/"*/ )
{
	if (!m_Loaded || ::strlen(nodeXPath) == 0)
	{
		return false;
	}

	if (m_document.first_element_by_path(nodeXPath).empty())
	{
		CompleteNodesOnXPath(nodeXPath, delims);
	}

	if (m_document.first_element_by_path(nodeXPath).attribute(nodeAttribute).empty())
	{
		m_document.first_element_by_path(nodeXPath).append_attribute(nodeAttribute);
	}

	m_document.first_element_by_path(nodeXPath).attribute(nodeAttribute).set_value(valueStr);

	m_document.save_file(m_xmlFilePath.c_str());

	return true;
}

void XmlTools::testXml()
{
	PrintNodeInfo(m_document.last_child());

	TraverseXmlNode("DataBaseConnect");

	//const char *ServerIP = GetValue("DataBaseConnect/ServerIP");
	//SetValue("DataBaseConnect/ServerIP", "127.0.0.1");
	//const char *ServerIP1 = GetValue("DataBaseConnect/ServerIP");

	//const char *version = GetAttribute("DataBaseConnect/DataBase", "name");
	//SetAttribute("DataBaseConnect", "version", "SqlServer2008");
	//const char *version1 = GetAttribute("DataBaseConnect/DataBase", "name");
}
