#include "DocumentControl.h"
#include <QUrl>

#include <Application.h>
#include <ApplicationModel.h>
#include <AppGlobal.h>
#include <FilesControl.h>


DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{
	QObject::connect(AppGlobal::app_mod, &ApplicationModel::onCurrentDocChanged, this, &DocumentControl::onCurrentDocChanged);
}

void DocumentControl::bindMathDocumentItem(MathDocumentCanvas* mathDocument)
{
	//cache math document and wait until it's ready
	m_doc_canvas = mathDocument;
	QObject::connect(m_doc_canvas, &MathDocumentCanvas::onNodeCreated, this, &DocumentControl::canvasReady, Qt::ConnectionType::DirectConnection);
}

void DocumentControl::keyInput(int key, const QString& text, int modifiers)
{	
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdoc_state = &m_visual_state;

	bool bShift = modifiers == Qt::Modifier::SHIFT;
	bool bCtrl = modifiers == Qt::Modifier::CTRL;
	switch (key) {
	case Qt::Key_Left:
		m_current_doc->stepLeft(bShift);
		m_current_doc->draw(&vt);
		updateElements(true, false, true);
		break;
	case Qt::Key_Right:
		m_current_doc->stepRight(bShift);
		m_current_doc->draw(&vt);
		updateElements(true, false, true);
		break;
	case Qt::Key_Up:
		//doc->StepY(-1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Down:
		//doc->StepY(1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Backspace:
		m_visual_state.Clear(true, true);
		m_current_doc->delBackward();
		m_current_doc->draw(&vt);
		updateElements(true, true, true);
		break;
	case Qt::Key_Delete:
		m_visual_state.Clear(true, true);
		m_current_doc->delForward();
		m_current_doc->draw(&vt);
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
			m_visual_state.Clear(true, true);
			m_current_doc->type(text.toStdWString());
			m_current_doc->draw(&vt);
			updateElements(true, true, true);
		}
		break;
	}
}

void DocumentControl::canvasReady()
{
	//caching doc state ptr in MathDocument
	m_doc_canvas->setMeDocState(&m_visual_state);
	//disconnecting from "renderer ready"
	QObject::disconnect(m_doc_canvas, &MathDocumentCanvas::onNodeCreated, this, &DocumentControl::canvasReady);
	QObject::connect(m_doc_canvas, &MathDocumentCanvas::onResized, this, &DocumentControl::onResized);
	m_is_canvas_ready = true;
}

MathElementInfoModel* DocumentControl::getMeInfoModel()
{
	if (m_meInfoModel)
	{
		//model is created->return it
		return m_meInfoModel;
	}
	//model not created yet-> create it
	m_meInfoModel = new MathElementInfoModel(this);
	/*auto& meList = FTAMeHelpers::GetMathElementsList();
	for (auto& me : meList)
	{
		m_meInfoModel->addMathElementInfo(MathElementInfo(QString::fromStdWString(me.first), ""));
	}*/
	return m_meInfoModel;
}

void DocumentControl::addMeByName(const QString& meName)
{
	
}

void DocumentControl::onCurrentDocChanged()
{
	m_current_doc = AppGlobal::app_mod->getCurrentDoc();
	if (m_is_canvas_ready)
	{
		VisualToolkit vt;
		vt.ft = AppGlobal::application->getFreeTypeWrap();
		vt.mdoc_state = &m_visual_state;
		m_current_doc->draw(&vt);
	}
}

void DocumentControl::onResized(const QSize& new_size)
{
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdoc_state = &m_visual_state;
	m_current_doc->setDocSize({ new_size.width(), new_size.height()});
	m_current_doc->draw(&vt);
	m_doc_canvas->update();
}

float DocumentControl::getScrollHandleSize()
{
	return 0.7f;
}

void DocumentControl::scrollY(bool Up)
{
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdoc_state = &m_visual_state;
	m_current_doc->scroll(Up);
	m_current_doc->draw(&vt);
	m_doc_canvas->update();
}

void DocumentControl::moveScrollHandle(float newPos)
{
	
}

void DocumentControl::mouseBtnDown(float x, float y)
{
	m_current_doc->stopSelection();
	m_bLmbDown = true;
	m_current_doc->updateSelection({ x,y });
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdoc_state = &m_visual_state;
	m_current_doc->draw(&vt);
	updateElements(true, false, true);
}

void DocumentControl::mouseBtnUp(float x, float y)
{
	
}

void DocumentControl::mousePosUpdated(float x, float y)
{
	m_current_doc->updateSelection({ x,y });
	VisualToolkit vt;
	vt.ft = AppGlobal::application->getFreeTypeWrap();
	vt.mdoc_state = &m_visual_state;
	m_current_doc->draw(&vt);
	updateElements(true, false, true);
}

void DocumentControl::updateElements(bool bRect, bool bText, bool bCaret)
{
	m_doc_canvas->update();
}

void DocumentControl::clearDocument()
{
	//clearing render data
	m_visual_state.Clear(true, true);
	//move caret outside visible area
	m_visual_state.SetCaret({ {-100, -100}, {1,1} });
	m_doc_canvas->update();
}



