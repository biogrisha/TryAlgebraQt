#include "DocumentControl.h"
#include <QUrl>

#include <Application.h>
#include <ApplicationModel.h>
#include <AppGlobal.h>
#include <FilesControl.h>
#include <Models/DocumentsModel.h>

DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{
	DocumentsModel* docModel = AppGlobal::appMod->docModel();
	QObject::connect(docModel, &DocumentsModel::onCurrentDocChanged, this, &DocumentControl::onCurrentDocChanged);
	QObject::connect(docModel, &DocumentsModel::onBeforeDocRemoved, this, &DocumentControl::onBeforeDocRemoved);
}

void DocumentControl::bindMathDocumentItem(MathDocumentCanvas* mathDocument)
{
	//cache math document and wait until it's ready
	m_docCanvas = mathDocument;
	QObject::connect(m_docCanvas, &MathDocumentCanvas::onNodeCreated, this, &DocumentControl::canvasReady, Qt::ConnectionType::DirectConnection);
}

void DocumentControl::keyInput(int key, QString text, int modifiers)
{	
	if (!m_currDoc)
	{
		return;
	}
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &m_visual_state;

	bool bShift = modifiers == Qt::Modifier::SHIFT;
	bool bCtrl = modifiers == Qt::Modifier::CTRL;
	switch (key) {
	case Qt::Key_Left:
		m_currDoc->step(TryAlgebraCore::StepDir::left, bShift);
		m_currDoc->draw();
		updateElements(true, false, true);
		break;
	case Qt::Key_Right:
		m_currDoc->step(TryAlgebraCore::StepDir::right, bShift);
		m_currDoc->draw();
		updateElements(true, false, true);
		break;
	case Qt::Key_Up:
		m_currDoc->step(TryAlgebraCore::StepDir::up, bShift);
		m_currDoc->draw();
		updateElements(true, false, true);
		break;
	case Qt::Key_Down:
		m_currDoc->step(TryAlgebraCore::StepDir::down, bShift);
		m_currDoc->draw();
		updateElements(true, false, true);
		break;
	case Qt::Key_Backspace:
		m_visual_state.Clear(true, true);
		m_currDoc->delBackward();
		m_currDoc->draw();
		updateElements(true, true, true);
		break;
	case Qt::Key_Delete:
		m_visual_state.Clear(true, true);
		m_currDoc->delForward();
		m_currDoc->draw();
		updateElements(true, true, true);
		break;
	case Qt::Key_Z:
		if (bCtrl)
		{
			//doc->Undo();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_Y:
		if (bCtrl)
		{
			//doc->Redo();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_C:
		if (bCtrl)
		{
			//doc->CopySelected();
			break;
		}
		[[fallthrough]];
	case Qt::Key_X:
		if (bCtrl)
		{
			//doc->CutSelected();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_V:
		if (bCtrl)
		{
			//doc->Paste();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	default:
		if (!text.isEmpty() && text != "\b")
		{
			if (text == "\r")
			{
				text = "\n";
			}
			m_visual_state.Clear(true, true);
			m_currDoc->type(text.toStdWString());
			m_currDoc->draw();
			updateElements(true, true, true);
		}
		break;
	}
}

void DocumentControl::canvasReady()
{
	//caching doc state ptr in MathDocument
	m_docCanvas->setMeDocState(&m_visual_state);
	//disconnecting from "renderer ready"
	QObject::disconnect(m_docCanvas, &MathDocumentCanvas::onNodeCreated, this, &DocumentControl::canvasReady);
	QObject::connect(m_docCanvas, &MathDocumentCanvas::onResized, this, &DocumentControl::onResized);
	m_isCanvasReady = true;
}

void DocumentControl::addMeByName(const QString& meName)
{
	if (m_currDoc)
	{
		m_currDoc->typeByName(meName.toStdWString());
		m_currDoc->draw();
		updateElements(true, true, true);
	}
}

void DocumentControl::onCurrentDocChanged(const QString& docPath)
{
	DocumentsModel* docModel = AppGlobal::appMod->docModel();
	DocumentInfo* docInfo = docModel->docInfo(docPath);
	m_currDoc = docInfo->meDoc();
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &m_visual_state;
	m_currDoc->setVisualToolkit(vt);
	if (m_isCanvasReady)
	{
		const QSize size = m_docCanvas->getSize();
		m_currDoc->setDocSize({ size.width(), size.height()});
		m_currDoc->draw();
		updateElements(true, true, true);
	}
}

void DocumentControl::onBeforeDocRemoved(DocumentInfo* docInfo)
{
	if (m_currDoc == docInfo->meDoc())
	{
		m_currDoc = nullptr;
		clearDocument();
	}
}

void DocumentControl::onResized(const QSize& new_size)
{
	if (!m_currDoc)
	{
		return;
	}
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &m_visual_state;
	m_currDoc->setDocSize({ new_size.width(), new_size.height()});
	m_currDoc->draw();
	m_docCanvas->update();
}

float DocumentControl::getScrollHandleSize()
{
	return 0.7f;
}

void DocumentControl::scrollY(bool Up)
{
	if (!m_currDoc)
	{
		return;
	}
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &m_visual_state;
	m_currDoc->scroll(Up);
	m_currDoc->draw();
	m_docCanvas->update();
}

void DocumentControl::moveScrollHandle(float newPos)
{
	if (!m_currDoc)
	{
		return;
	}
}

void DocumentControl::mouseBtnDown(float x, float y)
{
	if (!m_currDoc)
	{
		return;
	}
	m_currDoc->stopSelection();
	m_bLmbDown = true;
	m_currDoc->updateSelection({ x,y });
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &m_visual_state;
	m_currDoc->draw();
	updateElements(true, false, true);
}

void DocumentControl::mouseBtnUp(float x, float y)
{
	
}

void DocumentControl::mousePosUpdated(float x, float y)
{
	if (!m_currDoc)
	{
		return;
	}
	m_currDoc->updateSelection({ x,y });
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdocState = &m_visual_state;
	m_currDoc->draw();
	updateElements(true, false, true);
}

void DocumentControl::updateElements(bool bRect, bool bText, bool bCaret)
{
	m_docCanvas->update();
}

void DocumentControl::clearDocument()
{
	//clearing render data
	m_visual_state.Clear(true, true);
	//move caret outside visible area
	m_visual_state.SetCaret({ {-100, -100}, {1,1} });
	m_docCanvas->update();
}



