#pragma once
#include <QList>
#include <QHash>
#include <QFormLayout>
#include <QFrame>
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QToolBox>
#include <QDebug>
#include <QSlider>
#include <QDockWidget>

/// @brief The generic (pure virtual) superclass for all parameters
/// @todo use "#ifdef QT_GUI_LIB" to remove GUI code from this class
class Parameter : public QObject{
    friend class Parameters;
protected:
    QString  _label;
    /// GUI initialization must be done in constructor
    Parameter(QString label) : _label(label){}
    virtual ~Parameter(){}
public:
    /// The name of the parameter
    virtual QString& label(){ return _label; }
    /// Prints parameter to a string
    virtual QString toString() = 0;
    /// Return a reference to a an *already* allocated widget
    /// Typically allocate it in the parameter constructor
    virtual QWidget* widget() = 0;
    /// Transfer data from "value" to "widget"
    virtual void update_gui() = 0;
    /// Transfer data from "widget" to "value"
    virtual void collect_gui() = 0;
};

/// @brief this is a collection of parameters
class Parameters : public QObject{
protected:
    friend class ParametersWidget;
    friend class ParameterGroup;

    /// stores parameters in order of insertion
    QList<Parameter*> _list;

public:
    /// @brief parameters must be parented
    Parameters(QObject* parent=NULL) : QObject(parent){}

    /// @brief asks every parameter to read values from its GUI
    void collect_gui(){
        // qDebug() << "ParametersWidget::collect_gui";
        foreach(Parameter* par, _list)
            par->collect_gui();
    }

    /// @brief prints out a string representation of the parameters
    QString toString(QString prefix=""){
        QString retval;
        foreach(Parameter* par, _list){
            retval.append( prefix + par->toString() + "\n" );
        }
        return retval;
    }

    /// @brief adds a parameter to the collection
    template<class T>
    Parameters& operator<<(T& rhs){
        /// The collection steals the ownership
        rhs.setParent(this);
        /// Keep local list
        _list.push_back(&rhs);
        return *this;
    }
};

/** @brief Constructs a widget allowing the "live" editing of parameters. This
 * contains a "form" and a "toolbox" of parameters. In the form we have a table
 * where we have on left side the parameter "label",  on the right side, the
 * parameter associated "widget" */
class ParametersWidget : public QWidget{
    Q_OBJECT
    Q_DISABLE_COPY(ParametersWidget)
private:
    QFormLayout* form; // set in constructor
    QToolBox* toolbox; // set in constructor
    Parameters* pars;  // set in load_parameters
public:
    /// @brief Collects the values of all parameters
    void collect_gui(){ if(pars!=NULL) pars->collect_gui(); }

public:
    ParametersWidget(QWidget* parent = NULL) : QWidget(parent), pars(NULL){
        QVBoxLayout* layout = new QVBoxLayout(this);
        form = new QFormLayout(NULL);
        toolbox = new QToolBox(this);
        setLayout(layout);
        layout->addLayout(form);
        layout->addWidget(toolbox);

        /// Setup layouts
        form->setAlignment(Qt::AlignTop);
        form->setLabelAlignment(Qt::AlignLeft);
        form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    }

    void load_parameters(Parameters& pars){
        this->pars = &pars;
        foreach(Parameter* par, pars._list){
            bool isgroup = ( strcmp( par->metaObject()->className(), "ParameterGroup" ) == 0 );

            /// Stuff not in groups are shared
            if(!isgroup){
                /// with empty labels, take all row!!
                if(!par->label().isEmpty())
                    form->addRow(par->label(),par->widget());
                else
                    form->addRow(par->widget());
            }
            else {
                toolbox->addItem(par->widget(), par->label());
            }
        }
    }
};

/** @brief A dialog window allowing live-editing of parameters.
 *  It offers an "Apply" and "Cancel" buttons to which you can build
 *  connections with the given signals
 */
class ParametersDialog : public QWidget{
    Q_OBJECT
    Q_DISABLE_COPY(ParametersDialog)

protected:
    ParametersWidget pWidget;
    QPushButton      applyButton;
    QPushButton      cancelButton;
    QVBoxLayout      wlayout; /// Core widget layout
    QHBoxLayout      hlayout; /// Horizontal layout to store Buttons

signals:
    void applyReleased();
    void cancelReleased();

public:
    ParametersDialog(Parameters& pars)
        : applyButton("Apply",NULL),
          cancelButton("Cancel",NULL),
          wlayout(this),
          hlayout(NULL)
    {
        pWidget.load_parameters(pars);

        /// Tweaks layouts to get a "tight" GUI
        wlayout.setMargin(0);  ///< frame padding
        hlayout.setMargin(0);  ///< frame padding
        hlayout.setSpacing(1); ///< button padding

        /// Cannot go below this
        this->setMinimumWidth(200);
        this->setMinimumHeight(100);

        // Tell window to be as small as possible (doesn't work well, sets it to bigger than it could)
        // QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        // this->setSizePolicy(policy);

        /// Add items to layout
        wlayout.addWidget(&pWidget);
        wlayout.addLayout(&hlayout);
        hlayout.addWidget(&cancelButton);
        hlayout.addWidget(&applyButton);

        /// Connect button listener
        connect(&applyButton, SIGNAL(released()),this,SIGNAL(applyReleased()));
        connect(&cancelButton, SIGNAL(released()),this,SIGNAL(cancelReleased()));
    }
};

/// @brief A dockable widget allowing live-editing of parameters.
class ParametersDockWidget : public QDockWidget{
private:
    ParametersWidget _widget;
public:
    ParametersDockWidget(){
        QFrame* frame = new QFrame(this);
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment( Qt::AlignTop );
        frame->setLayout(layout);
        this->setWidget(frame);
        layout->addWidget(&_widget);        
    }
    void load_parameters(Parameters& pars){
        _widget.load_parameters(pars);
    }
};

//----------------------------------------------------------------------------//
//
//
//
//                              SPECIALIZATIONS
//
//
//
//----------------------------------------------------------------------------//

/// @brief The specialized (with templates) superclass for all parameters.
///        This template is simply used to reduce the amount of code needed.
/// @internal Unfortunately, we cannot call the pure virtual methods in this context
template< class _Value, class _Widget >
class TemplateParameter : public Parameter{
protected:
    typedef TemplateParameter<_Value, _Widget> Super;
protected:
    _Value* _value;       ///< we receive a reference
    _Widget* _widget; ///< widget created to visualize the value
protected:
    /// For conveniency, the templated superclass (java style)
    TemplateParameter(QString label) : Parameter(label){
        this->_value = NULL;
        this->_widget = NULL;
    }
    /// Constructor, reused by subclasses
    TemplateParameter(_Value& _value, QString label) : Parameter(label){
        this->_value = &_value;
        this->_widget = NULL;
    }
    /// Return the address of the widget (do not allocate memory!!!)
    /// instead, perform the "new QWidget" in the constructor
    _Widget* widget(){
        if(_widget==NULL)
            _widget = new _Widget();
        return _widget;
    }
    /// Returns a reference to the value stored
    _Value& value(){ return *_value; }
    /// @internal must be done in template, otherwise we don't know what a "value" is
    QString toString(){ return (_value==NULL) ? QString("Value '%1'=NULL").arg(_label) : QString("%1").arg(value()); }
};

/// @brief a static QLabel linked to a "QString"
class DescriptionBox : public TemplateParameter<QString, QLabel>{
    Q_OBJECT
public:
    static DescriptionBox& New(QString value){ return *new DescriptionBox(value); }
public slots:
    void update_gui(){ widget()->setText(_text); }
    void collect_gui(){}
private:
    QString _text; ///< local instance
    DescriptionBox(QString text) : Super(""){
        _text = text;
        _value = &_text;
        widget()->setWordWrap(true);
        update_gui();
    }
};

/// @brief a QCheckBox linked to a "bool"
class BooleanCheckbox : public TemplateParameter<bool, QCheckBox>{
    Q_OBJECT
public:
    static BooleanCheckbox& New(bool& value, QString label){ return *new BooleanCheckbox(value, label); }
    QString toString() { return (value()==true)?"true":"false"; }
signals:
    void toggled(bool);
public:
    BooleanCheckbox& connect_to(const QObject* receiver, const char* signalname){
        connect( this, SIGNAL(toggled(bool)), receiver, signalname );
        return (*this);
    }
public slots:
    void update_gui(){ widget()->setChecked(value()); }
    void collect_gui(){ value() = widget()->isChecked(); }
private:
    BooleanCheckbox(bool& value, QString label) : Super(value, label){
        update_gui();
        connect( widget(), SIGNAL(clicked()), this, SLOT(collect_gui()));
        connect( widget(), SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    }

};

/// @brief a QBox linked to an "int"
class IntegerSpinEdit : public TemplateParameter<int, QSpinBox>{
    Q_OBJECT
public:
    static IntegerSpinEdit& New(int& value, QString label){ return *new IntegerSpinEdit(value, label); }
public slots:
    void update_gui(){ widget()->setValue( value() ); }
    void collect_gui(){ value() = widget()->value(); }
    void intchanged(int val){qDebug()<<"fire"<<val;}
signals:
    void valueChanged(int);
public:
    IntegerSpinEdit& connect_to(const QObject* receiver, const char* signalname){
        connect( this, SIGNAL(valueChanged(int)), receiver, signalname);
        qDebug()<<"connected";
        return (*this);
    }
public:
    IntegerSpinEdit& setRange(int min, int max){
        widget()->setRange(min, max);
        /// @todo value() to fit criteria above
        // collect_gui() ///< not working!!
        return *this;
    }
private:
    IntegerSpinEdit(int& value, QString label) : Super(value, label){
        update_gui();
        /// makes it take a full line
        QSizePolicy policy = widget()->sizePolicy();
        policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
        widget()->setSizePolicy(policy);
        /// connects
        connect( widget(), SIGNAL(valueChanged(int)), this, SLOT(collect_gui()) );
        connect( widget(), SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)) );
    }
};

/// @brief a QSlider linked to an "int"
class IntegerSliderEdit : public TemplateParameter<int, QSlider>{
    Q_OBJECT
public:
    static IntegerSliderEdit& New(int& value, QString label){
        return *new IntegerSliderEdit(value, label);
    }
public slots:
    void update_gui(){ widget()->setValue( value() ); }
    void collect_gui(){ value() = widget()->value(); }
signals:
    void valueChanged(int);
public:
    IntegerSliderEdit& connect_to(const QObject* receiver, const char* signalname){
        connect( this, SIGNAL(valueChanged(int)), receiver, signalname);
        qDebug()<<"connected";
        return (*this);
    }
public:
    IntegerSliderEdit& setRange(int min, int max){
        widget()->setRange(min, max);
        /// @todo value() to fit criteria above
        // collect_gui() ///< not working!!
        return *this;
    }
    IntegerSliderEdit& setOrientation(Qt::Orientation o){
        widget()->setOrientation(o);
        return *this;
    }

private:
    IntegerSliderEdit(int& value, QString label) : Super(value, label){
        update_gui();
        /// makes it take a full line
        QSizePolicy policy = widget()->sizePolicy();
        policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
        widget()->setSizePolicy(policy);
        /// connects
        connect( widget(), SIGNAL(valueChanged(int)), this, SLOT(collect_gui()) );
        connect( widget(), SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)) );
    }
};
/// @brief a QSlider linked to an "double"
/// @todo what to do with round-off that might happen during initialization!?!
class DoubleSliderEdit : public TemplateParameter<double, QSlider>{
    Q_OBJECT
private:
    double _min; ///< 0.0
    double _max; ///< 1.0
public:
    static DoubleSliderEdit& New(double& value, QString label){
        Q_ASSERT(value>=0.0 && value<=1.0);
        return *new DoubleSliderEdit(value, 0.0, 1.0, 100, label);
    }
    static DoubleSliderEdit& New(double& value, double min, double max, QString label){
        Q_ASSERT(value>=min && value<=max);
        DoubleSliderEdit* ptr = new DoubleSliderEdit(value, min, max, 100, label);
        return *ptr;
    }
public slots:
    void update_gui(){
        double ratio = ( value() - _min ) / (_max - _min);
        int ticks = qRound( widget()->maximum() * ratio );
        widget()->setValue( ticks ); 
    }
    void collect_gui(){ 
        int _value = widget()->value();
        double frac = double(_value) / double(widget()->maximum()); ///< [0,1]
        value() = (1.0-frac)*_min + (frac)*_max; ///< [_min,_max]
        emit valueChanged(value());
    }
signals:
    void valueChanged(double);
public slots:
    void valueChanged(int value){
        double frac = double(value) / double(widget()->maximum()); ///< [0,1]
        double val = (1.0-frac)*_min + (frac)*_max; ///< [_min,_max]
        emit valueChanged(val);
    }

public:
    DoubleSliderEdit& connect_to(const QObject* receiver, const char* signalname){
        connect( this, SIGNAL(valueChanged(double)), receiver, signalname);
        return (*this);
    }
public:
    DoubleSliderEdit& setRange(double min=0.0, double max=1.0){
        _min = min;
        _max = max;
        update_gui();
        return *this;
    }
    DoubleSliderEdit& setSteps(int steps){
        widget()->setRange(0, steps);
        /// @todo value() to fit criteria above
        return *this;
    }
    DoubleSliderEdit& setOrientation(Qt::Orientation o){
        widget()->setOrientation(o);
        return *this;
    }

private:
    DoubleSliderEdit(double& value, double min, double max, int ticks, QString label) : Super(value, label){
        _min = min;
        _max = max;
        /// makes it take a full line        
        widget()->setRange(0,ticks);
        QSizePolicy policy = widget()->sizePolicy();
        policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
        widget()->setOrientation(Qt::Horizontal);
        widget()->setSizePolicy(policy);
        /// update with initial value
        update_gui();        
        /// connects
        connect( widget(), SIGNAL(valueChanged(int)), this, SLOT(collect_gui()) );
    }
};



/// @brief a QLineEdit linked to a "double"
class DoubleLineEdit : public TemplateParameter<double, QLineEdit>{
    Q_OBJECT
public:
    static DoubleLineEdit& New(double& value, QString label){ return *new DoubleLineEdit(value, label); }
signals:
    void valueChanged(double);
public:
    DoubleLineEdit& connect_to(const QObject* receiver, const char* signalname){
        connect( this, SIGNAL(valueChanged(double)), receiver, signalname);
        return (*this);
    }
public slots:
    void update_gui(){ widget()->setText( QString::number( value() ) ); }
    void collect_gui(){
        bool ok;
        value() = widget()->text().toDouble(&ok);
        if(!ok)
            value() = std::numeric_limits<double>::quiet_NaN();
    }
private:
    DoubleLineEdit(double& value, QString label) : Super(value, label){
        update_gui();
        /// @todo allow only floats to be inserted (numbers and commas)
        // ....
        /// connects
        connect( widget(), SIGNAL(textChanged(QString)), this, SLOT(collect_gui()) );
        connect( widget(), SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    }
private slots:
    ///< converts a text change into a valueChange signal
    void textChanged(QString){ emit valueChanged( value() ); }
};
/// @brief something that can be used to group parameters, creating a tree-like structure
///        it is not reccomended to use more than 2 levels of the tree!!
class ParameterGroup : public Parameter{
    Q_OBJECT
    Q_DISABLE_COPY(ParameterGroup)
private:
    /// contained parameters
    Parameters inner;
    ParametersWidget* _widget;
    /// @todo store if the form is currently being displayed
    // bool isactivated;
private:
    ParameterGroup(QString label) : Parameter(label), inner(this), _widget(NULL){}
public:
    static ParameterGroup& New(QString label){ return *new ParameterGroup(label); }

    QString toString(){
        QString ret = label() + "\n";
        foreach(Parameter* par, inner._list)
            ret.append(par->toString() + "\n");
        return ret;
    }

public:
    ParametersWidget* widget(){
        if(_widget==NULL){
            _widget = new ParametersWidget(NULL);
            _widget->load_parameters(inner);
        }
        return _widget;
    }
public slots:
    void update_gui(){
        foreach(Parameter* par, inner._list)
            par->update_gui();
    }
    void collect_gui(){
        foreach(Parameter* par, inner._list)
            par->collect_gui();
    }

public:
    /// @brief adds a parameter to the collection
    template<class T>
    ParameterGroup& operator<<(T& rhs){ inner << rhs; return *this; }
};

class ParametersConnectionTester : public QObject{
    Q_OBJECT
public slots:
    void valueChanged(double value){ qDebug() << "double: " << value; }
};
