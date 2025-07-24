/**
 * @file main_window.cpp
 * @brief Sourcefile for the MainWindow class, providing the main GUI for a Finite State Machine (FSM) editor.
 *
 * This file defines and implements the MainWindow class, which serves as the primary interface for a graphical
 * FSM editor. It manages user interactions for creating, editing, and running FSMs, including
 * state and transition manipulation, FSM loading/saving, and communication with an external FSM process.
 *
 * @author Tomáš Bordák [xborda01]
 * @author Erik Roják [xrojak00]
 * @author Radim Dvořák [xdvorar00]
 */


#include "main_window.h"
#include "ui_main_window.h"
#include "automaton_model.h"
#include "state.h"
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainterPath>
#include <fstream>
#include <cmath>
#include <QDebug>
#include <QRadioButton>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setStyleSheet(
        "QToolBar { background: #333333; border: none; }"
        "QToolBar QToolButton { color: #FFFFFF; padding: 8px; font-size: 14px; }"
        "QComboBox { background: #FFFFFF; border: 1px solid #AAAAAA; padding: 6px; font-size: 14px; border-radius: 4px; }"
        "QLineEdit { background: #FFFFFF; border: 1px solid #AAAAAA; padding: 6px; font-size: 14px; border-radius: 4px; }"
        "QTextEdit { background: #F8F8F8; border: 1px solid #AAAAAA; font-family: 'Monospace'; font-size: 12px; border-radius: 4px; }"
        "QPushButton { background: #4CAF50; color: #FFFFFF; border: none; padding: 10px; font-size: 14px; border-radius: 4px; font: bold }"
        "QPushButton:hover { background: #45A049; }"
    );

    ui->delete_state->setStyleSheet(
        "QPushButton { background: #f44336 } QPushButton:hover { background: #d73833 }"
    );

    ui->delete_transition->setStyleSheet(
        "QPushButton { background: #f44336 } QPushButton:hover { background: #d73833 }"
    );

    ui->back_state_edit->setStyleSheet(
        "QPushButton { background: #9e9e9e } QPushButton:hover { background: #7e7e7e }"
    );

    ui->back_transition_edit->setStyleSheet(
        "QPushButton { background: #9e9e9e } QPushButton:hover { background: #7e7e7e }"
    );

    // init diagram view
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->diagram_view->setScene(scene);
    ui->diagram_view->setRenderHint(QPainter::Antialiasing);

    QSize size = ui->diagram_view->size();
    ui->diagram_view->scene()->setSceneRect(0, 0, size.width(), size.height());
    ui->diagram_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    ui->statusBar->showMessage("Ready");
    qDebug() << "MainWindow initialized";

    automatonModel_ = new AutomatonModel(ui->leftMenu, this);

    // 'FSM Settings' menu will be shown as default
    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
}

MainWindow::~MainWindow() {
    delete ui;
}

// OTHERS
void MainWindow::loadFsm() {
    // FIRST CHECK FOR EXISTING AUTOMATON
    if (automatonModel_->states().count()) {
        QMessageBox msgBox;
        msgBox.setText("Existing automaton will be deleted.");
        msgBox.setInformativeText("Are you sure you want to load new automaton?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Yes:
            break; // continue
        case QMessageBox::Cancel:
            return; // stop
        default:
            return;
        }
    }

    QString file = QFileDialog::getOpenFileName(this, "Load FSM", QString(), "Text files (*.fsm)");

    if (!file.isEmpty()) {
        QFile inFile(file);
        QString data;

        if (inFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&inFile);
            data = in.readAll();
            inFile.close();

            qDebug() << "loadFSM" << data;
        } else {
            qWarning() << "Failed to open file:" << file;
        }

        QGraphicsScene* scene = ui->diagram_view->scene();

        // clear model and scene for new automaton
        automatonModel_ = automatonModel_->reset();
        scene->clear();

        // LOAD DATA FROM FILE
        if(automatonModel_->loadData(&data)){
            qDebug() << "Succesfully loaded FSM:" << file;
            ui->logDisplay->append("Succesfully loaded FSM: " + file);

            // DISPLAY LOADED DATA TO USER
            setSettingsAndDisplayFsm(scene);
        } else {
            ui->logDisplay->append("Error: Failed to load FSM from " + file);
            qDebug() << "Failed to load FSM:" << file;
            QMessageBox::warning(this, "Error", "Failed to load FSM");
        }
    }
}

void MainWindow::setSettingsAndDisplayFsm(QGraphicsScene* scene)
{
    // FSM SETTINGS LEFT MENU INPUTS
    ui->name->setText(automatonModel_->name());

    ui->comment->setText(automatonModel_->comment());

    ui->in->setPlainText(automatonModel_->inputs().join('\n'));

    ui->out->setPlainText(automatonModel_->outputs().join('\n'));

    ui->var->setPlainText(automatonModel_->variables().join('\n'));

    // DISPLAY STATES TO SCENE
    for(State* state : automatonModel_->states()){
        scene->addItem(state);
        state->setPos(state->X(),state->Y()); // set position after the state is added to scene
    }

    // DISPLAY TRANSITIONS TO SCENE
    for(Transition* tr : automatonModel_->transitions()){
        scene->addItem(tr);
    }
}

void MainWindow::saveFsm() {
    if(!automatonModel_->startState()){
        QMessageBox::warning(this, "Error Start State", "Automaton is missing a start state. Double-click a state and click 'Set As Start'.");
        return;
    }

    QString file = QFileDialog::getSaveFileName(this, "Save FSM", QString(), "Text files (*.fsm)");

    qDebug() << "Saving to file" << file;
    if (!file.endsWith(".fsm")) file.append(".fsm");

    QString data = automatonModel_->saveData();

    QFile f(file);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&f);
        out << data;

        f.close();
    } else {
        QMessageBox::warning(this, "Save Error", "Unable to open the file for writing.");
        return;
    }

    ui->logDisplay->append("Saved FSM to: " + file);
    qDebug() << "Saved FSM to:" << file;
    QMessageBox::information(this, "Success", "FSM saved to " + file);
}

/********** TOOL BAR **********/
// CREATE FSM
void MainWindow::on_create_act_triggered()
{
    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
}

// LOAD FSM
void MainWindow::on_load_act_triggered()
{
    MainWindow::loadFsm();
}

// SAVE FSM
void MainWindow::on_save_act_triggered()
{
    MainWindow::saveFsm();
}

// START FSM
void MainWindow::on_start_act_triggered()
{
    QString codePath = automatonModel_->generateCode();

    if (!QFile::exists(codePath)) {
        QMessageBox::critical(this, "Error", "Code generation failed.");
        return;
    }

    QString exePath = codePath;
    exePath.replace(".cpp", "");

    QProcess compiler;
    QStringList args;
    args << codePath << "-o" << exePath << "-std=c++17";

    compiler.start("g++", args);
    compiler.waitForFinished();

    qDebug() << "G++ exit:" << compiler.exitCode();
    qDebug() << "G++ stderr:" << compiler.readAllStandardError();

    if (!QFile::exists(exePath)) {
        QMessageBox::critical(this, "Error", "Compilation failed:\n" + compiler.readAllStandardError());
        return;
    }

    // start fsm
    fsmProcess_ = new QProcess(this);
    fsmProcess_->setProgram(exePath);
    fsmProcess_->start();

    if (!fsmProcess_->waitForStarted()) {
        QMessageBox::critical(this, "Error", "Failed to start FSM process.");
        return;
    }

    // (Qt app --> fsm process) = process STDIN
    // (fsm process --> Qt app) = process STDOUT

    connect(fsmProcess_, &QProcess::readyReadStandardError, this, [=]() {
        qDebug() << "[FSM stderr]" << fsmProcess_->readAllStandardError();
    });

    connect(fsmProcess_, &QProcess::readyReadStandardOutput, this, [=]() {
        QByteArray output = fsmProcess_->readAllStandardOutput();
        QString outputStr = QString::fromUtf8(output); // QByteArray to QString
        receiveFromFsm(outputStr);
    });

    // POKE FSM PROCESS TO FLUSH ITS STDOUT (SO Qt GETS MESSAGES)
    QTimer* pokeTimer = new QTimer(this);
    connect(pokeTimer, &QTimer::timeout, this, [=]() {
        if (fsmProcess_->state() == QProcess::Running) {
            fsmProcess_->write("POKE\n");
        }
    });
    pokeTimer->start(500);

    // SET LEFT MENU FOR RUNTIME
    ui->leftMenu->setCurrentWidget(ui->runtime);
    ui->updateInputVariable->clear();
    ui->updateInputVariable->addItems(automatonModel_->inputs());

    // INPUTS VALUES TABLE IN LEFT MENU
    QTableWidget* tableInput = ui->inputTable;
    QStringList inputs = automatonModel_->inputs();

    tableInput->setRowCount(inputs.count());
    tableInput->setHorizontalHeaderLabels(QStringList() << "NAME" << "VALUE");

    // load inputs table
    for(int r = 0; r < inputs.count(); ++r){
        tableInput->setItem(r, 0, new QTableWidgetItem(inputs[r]));
        tableInput->setItem(r, 1, new QTableWidgetItem("none"));
    }

    // OUTPUTS VALUES TABLE IN LEFT MENU
    QTableWidget* tableOutput = ui->outputTable;
    QStringList outputs = automatonModel_->outputs();

    tableOutput->setRowCount(outputs.count());
    tableOutput->setHorizontalHeaderLabels(QStringList() << "NAME" << "VALUE");

    // load outputs table
    for(int r = 0; r < outputs.count(); ++r){
        tableOutput->setItem(r, 0, new QTableWidgetItem(outputs[r]));
        tableOutput->setItem(r, 1, new QTableWidgetItem("none"));
    }

}

/********** STATES **********/
// ADD STATE
void MainWindow::on_add_state_clicked()
{
    QGraphicsScene* scene = ui->diagram_view->scene();

    State* state = new State(0, 0, 60, 60, ui->leftMenu, automatonModel_, this);

    scene->addItem(state);
    automatonModel_->addState(state);
}

// SAVE STATE EDIT (NAME, ACTIONS)
void MainWindow::on_save_state_edit_clicked()
{
    QString newName = ui->state_name->text();
    QString newAction = ui->state_action->toPlainText();

    State* state = automatonModel_->selectedState();

    // check if this state name isnt already used by another state
    foreach(State* existingState, automatonModel_->states()){
        if(existingState != state && existingState->name() == newName){
            QMessageBox::warning(
                this,
                "Duplicate State Name",
                "A state with this name already exits. Choose different one."
            );
            return;
        }
    }

    state->setName(newName);
    state->setAction(newAction);
    state->update(); // update state so its newName is painted on it


    qDebug() << "State name changed to:" << newName;
    qDebug() << "State action changed to:" << newAction;

    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
    automatonModel_->updateAndSetNullptrSelectedState();
}

// DELETE STATE
void MainWindow::on_delete_state_clicked()
{
    QGraphicsScene* scene = ui->diagram_view->scene();
    State* state = automatonModel_->selectedState();

    if(state->hasTransitions()){
        QMessageBox::warning(
            this,
            "State Has Transitions",
            "This state is still attached with one or more transitons to other states."
        );
        return;
    }

    if(state == automatonModel_->startState()){
        automatonModel_->setStartState(nullptr);
    }

    scene->removeItem(state);
    automatonModel_->deleteState(state);

    qDebug() << "Deleted state:" << state->name();

    delete state;
    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
}

// SET SELECTED STATE AS START STATE
void MainWindow::on_set_start_state_clicked()
{
    State* state = automatonModel_->selectedState();
    automatonModel_->setStartState(state);
}

// GO BACK BUTTON ON STATE EDIT MENU
void MainWindow::on_back_state_edit_clicked()
{
    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
    automatonModel_->updateAndSetNullptrSelectedState();
}

/********** TRANSITIONS **********/
// START 'ADD TRANSITION' ACTION
void MainWindow::on_add_transition_clicked()
{
    if(automatonModel_->states().count() >= 2){
        isTransitionPending_ = true;
        statusBar()->showMessage("Click on the state transition will go FROM.");
    } else {
        statusBar()->showMessage("Could not add transition - less than 2 states defined.", 5000);
    }
}

// ADD TRANSITION LOGIC
void MainWindow::onStateClicked(State* clickedState)
{
    // 'add_transition' has been clicked and user clicked on the first state (state FROM)
    if(isTransitionPending_ && !stateFrom_){
        stateFrom_ = clickedState;

        qDebug() << "Transition FROM:" << stateFrom_->name();
        statusBar()->showMessage("Click on the state transition will go TO.");
        return;
    }

    // user has already selected state FROM, now he clicked on state TO
    if(isTransitionPending_ && stateFrom_){
        Transition* tr = new Transition(stateFrom_, clickedState, automatonModel_, ui->leftMenu);
        automatonModel_->addTransition(tr);

        QGraphicsScene* scene = ui->diagram_view->scene();
        scene->addItem(tr);

        stateFrom_->addOutgoingTransition(tr);
        clickedState->addIncomingTransition(tr);

        isTransitionPending_ = false;
        stateFrom_ = nullptr;

        statusBar()->clearMessage();
        qDebug() << "Transition TO:" << clickedState->name();
    }
}

// SAVE TRANSITION CONDITION
void MainWindow::on_save_transition_edit_clicked()
{
    QString newCondition = ui->transition_condition->toPlainText();

    Transition* tr = automatonModel_->selectedTransiton();
    tr->setCondition(newCondition);

    qDebug() << "Transition condition changed to:" << newCondition;

    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
    automatonModel_->updateAndSetNullptrSelectedTransition();
}

// DELETE TRANSITION
void MainWindow::on_delete_transition_clicked()
{
    QGraphicsScene* scene = ui->diagram_view->scene();
    Transition* tr = automatonModel_->selectedTransiton();

    scene->removeItem(tr);
    automatonModel_->deleteTransition(tr);

    tr->fromState()->removeOutgoingTransition(tr);
    tr->toState()->removeIncomingTransition(tr);

    qDebug() << "Deleted transtion: " << tr->fromState()->name() << "-->" << tr->toState()->name();

    delete tr;
    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
}

// GO BACK FROM TRANSITION EDIT
void MainWindow::on_back_transition_edit_clicked()
{
    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
    automatonModel_->updateAndSetNullptrSelectedTransition();
}

/********** FSM SETTINGS **********/
// SAVE SETTINGS
void MainWindow::on_save_settings_clicked()
{
    QString name = ui->name->text();
    automatonModel_->setName(name);

    QString comment = ui->comment->text();
    automatonModel_->setComment(comment);

    automatonModel_->resetInputs();
    automatonModel_->resetOutputs();
    automatonModel_->resetVariables();
    QStringList temp;

    QString vars = ui->var->toPlainText();
    temp = vars.split('\n');

    for(QString var : temp){
        automatonModel_->addVariable(var);
    }

    QString inputs = ui->in->toPlainText();
    temp = inputs.split('\n');

    for(QString input : temp){
        automatonModel_->addInput(input);
    }

    QString outputs = ui->out->toPlainText();
    temp = outputs.split('\n');

    for(QString output : temp){
        automatonModel_->addOutput(output);
    }

    statusBar()->showMessage("Settings succesfully saved.", 3000);
}

/********** RUNTIME **********/
// TERMINATE FSM
void MainWindow::on_terminateButton_clicked()
{
    fsmProcess_->write("TERMINATE\n");
    fsmProcess_->kill();
    automatonModel_->setCurrentState(nullptr);
    newLog("TERMINATED");

    ui->leftMenu->setCurrentWidget(ui->fsmSettings);
}

// INPUT ACTION (UPDATE INPUT VARIABLE IN FSM)
void MainWindow::on_updateInputButton_clicked()
{
    QString var = ui->updateInputVariable->currentText();
    QString value = ui->updateInputValue->text();

    if(var.isEmpty() || value.isEmpty()){
        return;
    }

    QString message = QString("INPUT:%1=%2\n").arg(var, value);

    fsmProcess_->write(message.toUtf8());
    fsmProcess_->waitForBytesWritten();

    newLog("INPUT: " + var + " = " + value);
    ui->updateInputValue->clear();
}

// PROCESS RECEIVED MESSAGE FSM --> Qt
void MainWindow::receiveFromFsm(const QString& message)
{
    qDebug() << "[FSM stdout]" << message;

    // start at CURRENT_STATE
    if (message.contains("CURRENT_STATE:")) {
        int startIndex = message.indexOf("CURRENT_STATE:") + 14;
        int endIndex = message.indexOf("\n", startIndex);
        QString state = message.mid(startIndex, endIndex - startIndex).trimmed();

        newLog(QString("CURRENT STATE: %1").arg(state));

        automatonModel_->setCurrentState(state);
    }

    // get the lines after current state
    QStringList lines = message.split("\n");

    for (const QString& line : lines) {
        // INPUT
        if (line.startsWith("INPUT:")) {
            QString input = line.mid(6);  // ignore "INPUT:" prefix
            QStringList parts = input.split('=');

            if (parts.size() == 2) {
                QString inputName = parts[0].trimmed();
                QString value = parts[1].trimmed();
                updateValue(inputName, value, ui->inputTable);
            }
        }
        // OUTPUT
        else if (line.startsWith("OUTPUT:")) {
            QString output = line.mid(7);  // ignore "OUTPUT:" prefix
            QStringList parts = output.split('=');

            if (parts.size() == 2) {
                QString outputName = parts[0].trimmed();
                QString value = parts[1].trimmed();
                updateValue(outputName, value, ui->outputTable);
            }
        }
    }
}

void MainWindow::updateValue(const QString& name, const QString& value, QTableWidget* table)
{
    for (int r = 0; r < table->rowCount(); ++r) {
        if (table->item(r, 0)->text() == name) {
            table->setItem(r, 1, new QTableWidgetItem(value));
            return;
        }
    }
}

// FUNCTION FOR DATETIME AND MESSAGE LOGGING
void MainWindow::newLog(const QString& message)
{
    QString current = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->logDisplay->append("[" + current + "]" + " " + message);
}
