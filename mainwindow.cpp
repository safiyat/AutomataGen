#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QRegExp re("[a-z*+()]+");
	QRegExpValidator *v = new QRegExpValidator(re);
	ui->input->setValidator(v);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::process()
{

	ui->tableWidget->setColumnCount(0);
	ui->tableWidget->setRowCount(0);
	ui->statusBar->clearMessage();

	QRegExp re("[_a-z]");

	if(!ui->input->text().contains(re))
	{
		ui->statusBar->showMessage("No input string");
		ui->tableWidget->setColumnCount(0);
		ui->tableWidget->setRowCount(0);
		return;
	}

	QByteArray text = ui->input->text().toAscii();
	int parenthesis, alcount, pointer, i;
	i = parenthesis = alcount = 0;

	if(text.contains("*+") || text.contains("+*"))
	{
		ui->statusBar->showMessage("Operators occuring together");
		ui->tableWidget->setColumnCount(0);
		ui->tableWidget->setRowCount(0);
		return;
	}

	while( i < text.length())
	{
		if(text.at(i) == '(')
			parenthesis++;
		else if(text.at(i) == ')')
			parenthesis--;
		if(parenthesis < 0 || text.contains("()"))
		{
			ui->statusBar->showMessage("Parentheses are not used correctly");
			ui->tableWidget->setColumnCount(0);
			ui->tableWidget->setRowCount(0);
			return;
		}

		if(isalpha(text.at(i)))
			alcount++;
		else if((text.at(i) == '*') || (text.at(i) == '+'))
		{
			if(alcount == 0)
			{
				ui->statusBar->showMessage("Operators occur before input symbols");
				ui->tableWidget->setColumnCount(0);
				ui->tableWidget->setRowCount(0);
				return;
			}
			if(text.at(i - 1) != ')' && !isalpha(text.at(i - 1)))
			{
				ui->statusBar->showMessage("Operators occur against no input symbols");
				ui->tableWidget->setColumnCount(0);
				ui->tableWidget->setRowCount(0);
				return;
			}
		}
		i++;
	}

	if(alcount == 0 || parenthesis != 0)
	{
		ui->statusBar->showMessage("No input symbols OR incorrect usage of parentheses");
		ui->tableWidget->setColumnCount(0);
		ui->tableWidget->setRowCount(0);
		return;
	}

	QString inputSymbols;
	i = 0;
	while(i < text.length())
	{
		if(text.at(i) < 97)
		{
			i++;
			continue;
		}
		if(i == 0)
			inputSymbols.append(text.at(i));
		else
		{
			if (!inputSymbols.contains(text.at(i)))
				inputSymbols.append(text.at(i));
		}
		i++;
	}

	QStringList list;

	ui->tableWidget->setColumnCount(inputSymbols.length() + 1);

	list.append(trUtf8("ε"));

	for(char i = 0; i < inputSymbols.length(); i++)
	{
		list.append(QString::fromLocal8Bit(inputSymbols.toAscii().data() + i, 1));
		ui->tableWidget->setColumnWidth(i, 500 / ui->tableWidget->columnCount());
	}

	ui->tableWidget->setHorizontalHeaderLabels(list);

	list.clear();

/**Work begins**/
	int charCount, immCharCount, rowCount, enc;
	QChar ch;
	charCount = immCharCount = rowCount = enc = 0;
	for(int i = 0; i < text.length(); i++)
	{
		ch = text.at(i);
		if(ch > 96)//Is a normal valid input symbol
		{
		/*
			if(enc == 0)
				ui->tableWidget->setRowCount(++rowCount);
			else
				enc = 0;
			ui->tableWidget->setItem(rowCount - 1, inputSymbols.indexOf(ch) + 1, new QTableWidgetItem(tr("q%1").arg(rowCount)));
			charCount++;
			immCharCount = 1;
		*/
			ui->tableWidget->setRowCount(++rowCount);
			ui->tableWidget->setItem(rowCount - 1, inputSymbols.indexOf(ch) + 1, new QTableWidgetItem(tr("q%1").arg(rowCount)));
			charCount++;
			immCharCount = 1;
		}
		else if(ch == '*')
		{
			int k = i - 2;
			if(text.at(i - 1) == ')')//The regex is of the form ...(kl...p)*...
			{
				k = i - 1;
				parenthesis = immCharCount = 0;
				do
				{
					if(text.at(k) == ')')
						parenthesis++;
					else if(text.at(k) == '(')
						parenthesis--;
					else if(text.at(k) > 96)
						immCharCount++;
					k--;
				}while(parenthesis);
			}
/***************************************************************************************
			ui->statusBar->showMessage(tr("The value of k is %1, and it points to the character %2, which should precede the substring %3").arg(k).arg(QString(text.mid(k, 1))).arg(QString(text.mid(k+1, i - k))));
***************************************************************************************/
			if(text.at(k) == '*')//If a consecutive * operation, separation is required.
			{
				if(enc == 0)
					ui->tableWidget->insertRow(rowCount++ - immCharCount);//Separation state's row.
				else
					enc = 0;
				ui->tableWidget->setItem(rowCount - immCharCount - 1, 0, new QTableWidgetItem(tr("q%1").arg(rowCount - immCharCount))); //One way ε transition to next state, to create the separation.
			}

			//Forward ε transition, skipping symbol.
			if(ui->tableWidget->item(rowCount - immCharCount, 0) == 0)
			{
				ui->tableWidget->setItem(rowCount - immCharCount, 0, new QTableWidgetItem(tr("q%1").arg(rowCount)));
			}
			else
			{
				ui->tableWidget->setItem(rowCount - immCharCount, 0, new QTableWidgetItem(ui->tableWidget->item(rowCount - immCharCount, 0)->text().append(tr("q%1").arg(rowCount))));
			}

			//Backward ε transition, retracing symbol.
			ui->tableWidget->setRowCount(++rowCount);
			enc = 1;
			if(ui->tableWidget->item(rowCount - 1, 0) == 0)
			{
				ui->tableWidget->setItem(rowCount - 1, 0, new QTableWidgetItem(tr("q%1").arg(rowCount - immCharCount - 1)));
			}
			else
			{
				ui->tableWidget->setItem(rowCount - 1, 0, new QTableWidgetItem(ui->tableWidget->item(rowCount - 1, 0)->text().append(tr("q%1").arg(rowCount - immCharCount - 1))));
			}
		}
	}


/**Final Stuff**/
	if(enc == 0)
		ui->tableWidget->setRowCount(++rowCount);

	for(int i = 0; i < ui->tableWidget->rowCount(); i++)
		list.append(tr("q%1").arg(i));
	ui->tableWidget->setVerticalHeaderLabels(list);
	for(int r = 0; r < rowCount; r++)
	{
		for(int c = 1; c < ui->tableWidget->columnCount(); c++)
		{
			if(ui->tableWidget->item(r, c) != 0)
			{
				ui->tableWidget->setItem(r, c, new QTableWidgetItem(tr("q%1").arg(r+1)));
			}
		}
	}
	ui->tableWidget->setColumnWidth(ui->tableWidget->columnCount() - 1, ui->tableWidget->columnWidth(1) - 2);
}

void MainWindow::clearTable()
{
	ui->tableWidget->setColumnCount(0);
	ui->tableWidget->setRowCount(0);
	ui->statusBar->clearMessage();
}

/*
  a*bcd*(er)*q*y


			if(ui->tableWidget->item(rowCount - immCharCount, 0) == 0)
			{
				ui->tableWidget->setItem(rowCount - immCharCount, 0, new QTableWidgetItem(tr("q%1, Ficc%2").arg(rowCount + immCharCount - 1).arg(immCharCount)));
				ui->statusBar->showMessage(tr("1Set Ficc for %1, with data q%2 and Ficc%3").arg(rowCount - immCharCount).arg(rowCount + immCharCount - 1).arg(immCharCount));
			}
			else
			{
				ui->tableWidget->setItem(rowCount - immCharCount, 0, new QTableWidgetItem(ui->tableWidget->item(rowCount - immCharCount, 0)->text().append(tr(", q%1, Ficc%2").arg(rowCount + immCharCount - 1).arg(immCharCount))));
				ui->statusBar->showMessage(tr("2Set Ficc for %1, with data q%2 and Ficc%3").arg(rowCount - immCharCount).arg(rowCount + immCharCount - 1).arg(immCharCount));
			}
*/
