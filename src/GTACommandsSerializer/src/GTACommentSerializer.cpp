#include "GTACommentSerializer.h"
#include "GTAUtil.h"

GTACommentSerializer::GTACommentSerializer(GTACommandBase *ipComment) : GTACmdSerializer()
{
	_pComment = ipComment;
}

bool GTACommentSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oComment)//(QDomDocument &iDomDoc,QDomElement &oComment)
{
	bool rc = false;
	if(_pComment != NULL)
	{
          QString commentText = _pComment->getComment();
          commentText.replace("\n","<NEW_LINE/>");
          oComment = iDomDoc.createElement(XNODE_COMMENT);
		  QString temp = QString("\"%1\"").arg(commentText);
          QDomText domCommentText = iDomDoc.createTextNode(QString("\"%1\"").arg(commentText));
          oComment.appendChild(domCommentText);
		rc = true;
	}
	return rc;
}

bool GTACommentSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioComment)
{
	bool rc = false;
     if(!iElement.isNull() && opCommand != NULL)
	{
         rc = true;
         QDomNodeList childList = iElement.childNodes();
         for(int i = 0 ; i < childList.count(); i++)
         {

             QDomNode domNode = childList.at(i);
             if(domNode.isElement() && domNode.nodeName() == XNODE_COMMENT)
             {
                QDomElement domElem = domNode.toElement();
                QString commentTxt = domElem.text();
				if (!commentTxt.isEmpty())
				{
					commentTxt.replace("<NEW_LINE/>","\n");
					commentTxt = commentTxt.remove("\"");
				}
                opCommand->setComment(commentTxt);
             }
         }
	}
	return rc;

}

