#include "ProgressBar.h"

namespace Isis {

  /**
   *
   */
  ProgressBar::ProgressBar(QWidget *parent): QProgressBar(parent) {
  }

  /**
   *
   */
  ProgressBar::ProgressBar(QString textDescription,
      QWidget *parent): QProgressBar(parent) {
    m_customText = textDescription;
  }


  /**
   * Free the allocated memory by this object
   */
  ProgressBar::~ProgressBar() {
  }


  /**
   * Set custom text for this progress bar. This will appear before the
   *   ##%.
   */
  void ProgressBar::setText(QString text) {
    m_customText = text;
  }


  /**
   * This applies the custom text.
   */
  QString ProgressBar::text() const {
    QString progressText = m_customText;

    if(progressText.size() != 0)
      progressText += " ";

    return progressText + QProgressBar::text();
  }
}
