#include "DakeCoinamountfield.h"

#include "qvaluecombobox.h"
#include "DakeCoinunits.h"
#include "guiconstants.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QDoubleSpinBox>
#include <QApplication>
#include <qmath.h> // for qPow()

DakeCoinAmountField::DakeCoinAmountField(QWidget *parent):
        QWidget(parent), amount(0), currentUnit(-1)
{
    amount = new QDoubleSpinBox(this);
    amount->setLocale(QLocale::c());
    amount->setDecimals(8);
    amount->installEventFilter(this);
    amount->setMaximumWidth(170);
    amount->setSingleStep(1.0);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(amount);
    unit = new QValueComboBox(this);
    unit->setModel(new DakeCoinUnits(this));
    layout->addWidget(unit);
    layout->addStretch(1);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(amount);

    // If one if the widgets changes, the combined content changes as well
    connect(amount, SIGNAL(valueChanged(QString)), this, SIGNAL(textChanged()));
    connect(unit, SIGNAL(currentIndexChanged(int)), this, SLOT(unitChanged(int)));

    // Set default based on configuration
    unitChanged(unit->currentIndex());
}

void DakeCoinAmountField::setText(const QString &text)
{
    if (text.isEmpty())
        amount->clear();
    else
        amount->setValue(text.toDouble());
}

void DakeCoinAmountField::clear()
{
    amount->clear();
    unit->setCurrentIndex(0);
}

bool DakeCoinAmountField::validate()
{
    bool valid = true;
    if (amount->value() == 0.0)
        valid = false;
    if (valid && !DakeCoinUnits::parse(currentUnit, text(), 0))
        valid = false;

    setValid(valid);

    return valid;
}

void DakeCoinAmountField::setValid(bool valid)
{
    if (valid)
        amount->setStyleSheet("");
    else
        amount->setStyleSheet(STYLE_INVALID);
}

QString DakeCoinAmountField::text() const
{
    if (amount->text().isEmpty())
        return QString();
    else
        return amount->text();
}

bool DakeCoinAmountField::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        // Clear invalid flag on focus
        setValid(true);
    }
    else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Comma)
        {
            // Translate a comma into a period
            QKeyEvent periodKeyEvent(event->type(), Qt::Key_Period, keyEvent->modifiers(), ".", keyEvent->isAutoRepeat(), keyEvent->count());
            QApplication::sendEvent(object, &periodKeyEvent);
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

QWidget *DakeCoinAmountField::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, amount);
    return amount;
}

qint64 DakeCoinAmountField::value(bool *valid_out) const
{
    qint64 val_out = 0;
    bool valid = DakeCoinUnits::parse(currentUnit, text(), &val_out);
    if(valid_out)
    {
        *valid_out = valid;
    }
    return val_out;
}

void DakeCoinAmountField::setValue(qint64 value)
{
    setText(DakeCoinUnits::format(currentUnit, value));
}

void DakeCoinAmountField::unitChanged(int idx)
{
    // Use description tooltip for current unit for the combobox
    unit->setToolTip(unit->itemData(idx, Qt::ToolTipRole).toString());

    // Determine new unit ID
    int newUnit = unit->itemData(idx, DakeCoinUnits::UnitRole).toInt();

    // Parse current value and convert to new unit
    bool valid = false;
    qint64 currentValue = value(&valid);

    currentUnit = newUnit;

    // Set max length after retrieving the value, to prevent truncation
    amount->setDecimals(DakeCoinUnits::decimals(currentUnit));
    amount->setMaximum(qPow(10, DakeCoinUnits::amountDigits(currentUnit)) - qPow(10, -amount->decimals()));

    if(currentUnit == DakeCoinUnits::uBTC)
        amount->setSingleStep(0.01);
    else
        amount->setSingleStep(0.001);

    if(valid)
    {
        // If value was valid, re-place it in the widget with the new unit
        setValue(currentValue);
    }
    else
    {
        // If current value is invalid, just clear field
        setText("");
    }
    setValid(true);
}

void DakeCoinAmountField::setDisplayUnit(int newUnit)
{
    unit->setValue(newUnit);
}
