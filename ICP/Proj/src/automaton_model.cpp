/**
 * @file automaton_model.cpp
 * @brief Implementation of the AutomatonModel class
 * @details This file contains the implementation of the AutomatonModel class,
 *          which represents the finite state machine (FSM) model.
 * 
 * @date 2023-05-11
 * 
 * @author Tomáš Bordák [xborda01]
 * @author Radim Dvořák [xdvorar00]
 * @author Erik Roják [xrojak00]
 */

#include "automaton_model.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

AutomatonModel::AutomatonModel(QStackedWidget* leftMenu, MainWindow* mainWindow)
    : leftMenu_(leftMenu), mainWindow_(mainWindow)
{
}

void AutomatonModel::addState(State* state)
{
    states_.append(state);
}

void AutomatonModel::deleteState(State* state)
{
    states_.removeOne(state);

    if(state == selectedState_){
        selectedState_ = nullptr;
    }
}

const QList<State*>& AutomatonModel::states() const
{
    return states_;
}

void AutomatonModel::setSelectedState(State* state)
{
    selectedState_ = state;
    qDebug() << "Selected state:" << state->name();
}

State* AutomatonModel::selectedState() const
{
    return selectedState_;
}

void AutomatonModel::setStartState(State* state)
{
    startState_ = state;

    foreach(State* st, states_){
        if(st == startState_){
            st->setBrush(Qt::green);
        } else {
            st->setBrush(Qt::lightGray);
        }
    }

    if(state != nullptr){
        qDebug() << "State set as start:" << state->name();
    } else {
        qDebug() << "State set as start: nullptr";
    }
}

State* AutomatonModel::startState()
{
    return startState_; // start state or nullptr
}

void AutomatonModel::setSelectedTransition(Transition* tr)
{
    selectedTransition_ = tr;
}

Transition* AutomatonModel::selectedTransiton() const
{
    return selectedTransition_;
}

void AutomatonModel::addTransition(Transition* tr)
{
    transitions_.append(tr);
}


void AutomatonModel::deleteTransition(Transition* tr)
{
    transitions_.removeOne(tr);

    if(tr == selectedTransition_){
        selectedTransition_ = nullptr;
    }
}

const QList<Transition*>& AutomatonModel::transitions() const
{
    return transitions_;
}

void AutomatonModel::updateAndSetNullptrSelectedTransition()
{
    if(!selectedTransition_) return;

    selectedTransition_->update(); // repaint
    selectedTransition_ = nullptr;
}

void AutomatonModel::updateAndSetNullptrSelectedState()
{
    if(!selectedState_) return;

    selectedState_->update(); // repaint border
    selectedState_ = nullptr;
}

void AutomatonModel::setName(QString& name)
{
    name_ = name;
}

QString AutomatonModel::name() const
{
    return name_;
}

void AutomatonModel::setComment(QString& comment)
{
    comment_ = comment;
}

QString AutomatonModel::comment() const
{
    return comment_;
}

void AutomatonModel::addInput(QString& input)
{
    inputs_.append(input);
}

QStringList AutomatonModel::inputs() const
{
    return inputs_;
}

void AutomatonModel::addOutput(QString& output)
{
    outputs_.append(output);
}

QStringList AutomatonModel::outputs() const
{
    return outputs_;
}

void AutomatonModel::addVariable(QString& variable)
{
    variables_.append(variable);
}

QStringList AutomatonModel::variables() const
{
    return variables_;
}

void AutomatonModel::resetInputs()
{
    inputs_.clear();
}

void AutomatonModel::resetOutputs()
{
    outputs_.clear();
}

void AutomatonModel::resetVariables()
{
    variables_.clear();
}

QString AutomatonModel::saveData() const
{
    QString result;
    QTextStream oss(&result);

    oss << "FSM Name:\n    "<< name() << "\n";
    oss << "Comment:\n    " << comment() << "\n";
    oss << "Inputs:\n";
    for (QString input : inputs()) {
        oss << "    " << input << "\n";
    }
    oss << "Outputs:\n";
    for (QString output : outputs()) {
        oss << "    " << output << "\n";
    }
    oss << "Variables (available at runtime):\n";
    for (QString var : variables()) {
        oss << "    " << var << "\n";
    }
    oss << "States and its actions (first one is start state):\n";

    oss << "    " << startState_->name();
    oss << " [" << startState_->pos().x() << ", " << startState_->pos().y() << "]";
    oss << " : { " << startState_->action().replace(";\n", ";") << " }";
    oss << "\n";

    for (const auto* state : states()) {
        if(state == startState_) continue;

        oss << "    " << state->name();
        oss << " [" << state->pos().x() << ", " << state->pos().y() << "]";
        oss << " : { " << state->action().replace(";\n", ";") << " }";
        oss << "\n";
    }

    oss << "Transitions with their conditions:\n";
    for (auto* trans : transitions()) {
        oss << "    " << trans->fromState()->name() << " --> " << trans->toState()->name() << ": ";
        QString condition = trans->condition();
        if (!condition.isEmpty()) {
            oss << " if( " << condition << " )";
        }
        oss << "\n";
    }

    return result;
}

bool AutomatonModel::loadData(QString* data)
{
    QTextStream iss(data);
    QString line;
    QString section;
    QMap<QString, State*> stateMap;

    while (!iss.atEnd()) {
        line = iss.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;

        // parse section header
        if (line == "FSM Name:") {
            section = "name";
            continue;
        } else if (line == "Comment:") {
            section = "comment";
            continue;
        } else if (line == "Inputs:") {
            section = "inputs";
            continue;
        } else if (line == "Outputs:") {
            section = "outputs";
            continue;
        } else if (line == "Variables (available at runtime):") {
            section = "variables";
            continue;
        } else if (line == "States and its actions (first one is start state):") {
            section = "states";
            continue;
        } else if (line == "Transitions with their conditions:") {
            section = "transitions";
            continue;
        }

        // then parse section data
        if (section == "name") {
            setName(line);
        } else if (section == "comment") {
            setComment(line);
        } else if (section == "inputs") {
            addInput(line);
        } else if (section == "outputs") {
            addOutput(line);
        } else if (section == "variables") {
            addVariable(line);
        } else if (section == "states") {
            // "NAME [x, y] : { action }"
            QRegularExpression re(R"(^(\w+)\s*\[(\d+),\s*(\d+)\]\s*:\s*\{\s*([^\}]*)\s*\})");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                QString stateName = match.captured(1).trimmed();
                qreal x = match.captured(2).toInt();
                qreal y = match.captured(3).toInt();
                QString action = match.captured(4).trimmed();

                State* state = new State(x, y, 60, 60, leftMenu_, this, mainWindow_);

                state->setName(stateName);
                if (!action.isEmpty()) {
                    action.replace(";", ";\n");
                    state->setAction(action);
                }

                addState(state);
                stateMap[stateName] = state;

                if(!startState()){
                    setStartState(state);
                }
            }
        } else if (section == "transitions") {
            // "FROM --> TO: if( condition )" or "FROM --> TO:"
            QRegularExpression re(R"(^(\w+)\s*-->\s*(\w+)\s*:\s*(?:if\s*\(\s*(.*?)\s*\))?\s*$)");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                QString fromName = match.captured(1).trimmed();
                QString toName = match.captured(2).trimmed();
                QString condition = match.captured(3).trimmed();

                State* fromState = stateMap.value(fromName);
                State* toState = stateMap.value(toName);

                if (fromState && toState) {
                    Transition* trans = new Transition(fromState, toState, this, leftMenu_);
                    if (!condition.isEmpty()) {
                        trans->setCondition(condition);
                    }

                    addTransition(trans);
                    fromState->addOutgoingTransition(trans);
                    toState->addIncomingTransition(trans);
                }
            }
        }
    }

    return !name_.isEmpty() && !states_.isEmpty();
}

AutomatonModel* AutomatonModel::reset()
{
    QStackedWidget* menu = leftMenu_;
    MainWindow* window = mainWindow_;
    delete this;

    return new AutomatonModel(menu, window);
}

void AutomatonModel::setCurrentState(const QString& stateName)
{
    if(stateName == nullptr){
        currentState_ = nullptr;
    }

    for (State* state : states_) {
        if (state->name() == stateName) {
            currentState_ = state;
            break;
        }
    }
}

State* AutomatonModel::currentState()
{
    return currentState_;
}

QString AutomatonModel::generateCode()
{
    QString filePath = QDir::temp().filePath("generated_fsm.cpp");
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not write FSM code to file.";
        return "";
    }

    QTextStream out(&file);

    out << R"(
    #include <iostream>
    #include <string>
    #include <unordered_map>
    #include <vector>
    #include <sstream>
    #include <regex>
    #include <chrono>
    #include <thread>

    using namespace std;

    struct Transition {
        string toState;
        string condition;
    };

    std::chrono::steady_clock::time_point stateStartTime;
    std::chrono::steady_clock::time_point fsmStartTime;

    unordered_map<string, vector<Transition>> transitions;
    unordered_map<string, string> stateActions;

    unordered_map<string, int> inputs;
    unordered_map<string, int> outputs;
    unordered_map<string, int> timeVars;

    string currentState;
    string updatedInput;

    void sendAll() {
        cout << "CURRENT_STATE:" << currentState << endl;

        for (const auto& input : inputs) {
            cout << "INPUT:" << input.first << "=" << input.second << endl;
        }

        for (const auto& output : outputs) {
            cout << "OUTPUT:" << output.first << "=" << output.second << endl;
        }
        cout.flush();
    }

    // get the string without whitespaces
    string trim(const string& s) {
        size_t start = s.find_first_not_of(" \t");
        size_t end = s.find_last_not_of(" \t");
        return (start == string::npos) ? "" : s.substr(start, end - start + 1);
    }

    void updateInput(const string& line) {
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string name = line.substr(0, pos);
            int value = stoi(line.substr(pos + 1));

            inputs[name] = value;
            updatedInput = name;
        }
    }

    bool evaluateCondition(const string& cond) {
        if (cond.empty()) return true;

        size_t pos = cond.find("==");
        if (pos != string::npos) {
            string input = trim(cond.substr(0, pos)); // get the input name without whitespaces
            int value = stoi(cond.substr(pos + 2));

            if (inputs.find(input) != inputs.end()) {
                return inputs[input] == value;
            }
        }

        return false;
    }

    bool isTransitionRelevant(const string& condition) {
        string input = updatedInput + " ";  // Appending space to ensure exact match

        if (condition.find(input) != string::npos) {
            return true;
        }
        return false;
    }

    void runStateActions(const string& actionCode) {
    )";

    out << R"cpp(
        std::regex re(R"re(output\s*\(\s*"([^"]+)"\s*,\s*(elapsed|-?\d+)\s*\)\s*;)re");
    )cpp";

    out << R"(
        auto begin = std::sregex_iterator(actionCode.begin(), actionCode.end(), re);
        auto end = std::sregex_iterator();

        for (auto it = begin; it != end; ++it) {
            string outputName = (*it)[1];
            string valueStr = (*it)[2];

            int value = 0;
            if (valueStr == "elapsed") {
                auto now = std::chrono::steady_clock::now();
                value = std::chrono::duration_cast<std::chrono::milliseconds>(now - fsmStartTime).count();
            } else {
                value = std::stoi(valueStr);
            }

        outputs[outputName] = value;
        cout << "OUTPUT:" << outputName << "=" << value << endl;
        }
    }

    void applyTransitions() {
        auto it = transitions.find(currentState);

        if (it != transitions.end()) {
            for (const auto& t : it->second) {
                // timed conditions
                if (!t.condition.empty() && t.condition[0] == '@') {
                    string varName = t.condition.substr(1);

                    int targetMs = 0;

                    auto it = timeVars.find(varName);
                    if (it != timeVars.end()) {
                        targetMs = it->second;
                    } else {
                        cerr << "ERROR: unknown time var @" << varName << endl;
                        continue;
                    }

                    auto now = std::chrono::steady_clock::now();
                    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateStartTime).count();

                    if (elapsed >= targetMs) {
                        cout << "TRANSITION (timed): " << currentState << " --> " << t.toState << endl;
                        currentState = t.toState;
                        stateStartTime = std::chrono::steady_clock::now(); // reset timer
                        runStateActions(stateActions[currentState]);
                        sendAll(); // send message after state change
                        break;
                    }
                } else if (isTransitionRelevant(t.condition)) {
                    if (evaluateCondition(t.condition)) {
                        cout << "TRANSITION: " << currentState << " --> " << t.toState << endl;
                        currentState = t.toState;
                        stateStartTime = std::chrono::steady_clock::now();
                        runStateActions(stateActions[currentState]);
                        sendAll();
                        break;
                    }
                }
            }
        }
    }
    )";

    out << R"(
    int main() {
    )";

    out << "    currentState = \"" << startState_->name() << "\";\n";

    for (const QString& input : inputs_) {
        out << "    inputs[\"" << input << "\"];\n";
    }

    for (const QString& output : outputs_) {
        out << "    outputs[\"" << output << "\"];\n";
    }

    for (const QString& var : variables_) {
        QStringList parts = var.split('=');
        if (parts.size() == 2) {
            QString name = parts[0].trimmed();
            QString value = parts[1].trimmed();
            out << "timeVars[\"" << name << "\"] = " << value << ";\n";
        }
    }

    for (State* state : states_) {
        QString action = state->action().trimmed();
        action.replace(";\n", ";");
        action.replace("\"", "\\\""); // escape double-qoutes
        out << "stateActions[\"" << state->name() << "\"] = \"" << action << "\";\n";
    }

    for (auto transition : transitions_) {
        QString from = transition->fromState()->name();
        QString to = transition->toState()->name();
        QString condition = transition->condition();
        out << "    transitions[\"" << from << "\"].push_back({\"" << to << "\", \"" << condition << "\"});\n";
    }

    out << R"(
        string line;
        stateStartTime = chrono::steady_clock::now();
        fsmStartTime = chrono::steady_clock::now();

        sendAll(); // send start state and init values

        while (true) {
            if (getline(cin, line)) {
                if (line.substr(0, 6) == "INPUT:") {
                    updateInput(line.substr(6));  // After "INPUT:"
                    applyTransitions(); // also run actions on currentState after transition
                } else if (line == "TERMINATE") {
                    break;
                }
            }

            applyTransitions(); // for timed transitions
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

    return 0;
    }
    )";

    file.close();
    return filePath;
}
