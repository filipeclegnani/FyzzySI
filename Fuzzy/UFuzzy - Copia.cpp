//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>

#pragma hdrstop

#include "UFuzzy.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TFmFuzzy *FmFuzzy;

//---------------------------------------------------------------------

std::vector <float> tip_cheap(31);
std::vector <float> tip_average(31);
std::vector <float> tip_gorgeous(31);
std::vector <float> tip(31);

float x=0;
float y=0;
float posicao_do_grafico = 0;
float posicao_do_grafico_food = 0;
float posicao_do_grafico_service = 0;

// Vari�veis Fuzzy.
float fiService = 0;
float fiFood    = 0;
float foTip     = 0;
float fop_rule1 = 0;
float fop_rule2 = 0;
float fop_rule3 = 0;

// Vari�veis de auxilio ao c�lculo da centr�ide.
float sum = 0;
float total_area = 0;
float gorjeta = 0;

// Entradas para o sistema.
float service = 0;
float food = 0;


//---------------------------------------------------------------------

// Fun��o Triangular
float trimf(float x, float a, float b, float c)
{
    float ua = 0;

    if (x <= a)
        ua = 0;
    else if ((a < x) && (x <= b))
        ua = ((x - a) / (b - a));
    else if ((b < x) && (x <= c))
        ua = ((x - c) / (b - c));
    else if (x > c)
        ua = 0;

    return(ua);
}

//---------------------------------------------------------------------

// Fun��o Trapezoidal
float trapmf(float x, float a, float b, float c, float d)
{
    float ua = 0;

    if (x <= a)
        ua = 0;
    else if ((a < x) && (x <= b))
        ua = ((x - a) / (b - a));
    else if ((b <= x) && (x <= c))
        ua = 1;
    else if ((c < x) && (x <= d))
        ua = ((d - x) / (d - c));
    else if (x > d)
        ua = 0;

    return(ua);
}

//---------------------------------------------------------------------

float min_val(float a, float b)
{
    if (a < b)
        return a;
    else
        return b;
}

//---------------------------------------------------------------------

float max_val(float a, float b)
{
	if (a > b)
		return a;
	else
        return b;
}


//---------------------------------------------------------------------------

void Fuzzy()
{

	// 1� regra - If service is poor or food is rancid, tip is cheap
	if ((service >= 0 && service <= 4) || (food >= 0 && food <= 3))
	{
		// Fuzzificar as entradas.
		fiService = trapmf(service,-1,0,1,4);
		fiFood    = trapmf(food,-1,0,1,3);

		// Aplica��o dos operadores Fuzzy.
		fop_rule1 = max_val(fiService,fiFood);

		// Aplica��o do M�todo de Implica��o (valores m�nimos).
		x=0;
		y=0;
		for (int a=0; a<=30; a++)
		{
			y = trimf(x,0,5,10);

			if (y >= fop_rule1)
			{
				tip_cheap.at(a) = fop_rule1;
			}
			else
			{
				tip_cheap.at(a) = y;
			}

			x=x+1;
		}
	}




	// 2� regra - If service is good, tip is average
	if ((service >= 1) && (service <= 9))
	{
		// Fuzzificar as entradas.
		fiService = trimf(service,1,5,9);

		// Aplica��o dos operadores Fuzzy.
		fop_rule2 = max_val(fiService,0);

		 // Aplica��o do M�todo de Implica��o (valores m�nimos).
		x=0;
		y=0;
		for (int a=0; a<=30; a++)
		{
			y = trimf(x,10,15,20);

			if (y >= fop_rule2)
			{
				tip_average.at(a) = fop_rule2;
			}
			else
			{
				tip_average.at(a) = y;
			}

			x=x+1;
		}
	}




	// 3� regra - If service is excellent or food is delicious, tip is generous
	if ((service >= 6 && service <= 10) || (food >= 7 && food <= 10))
	{
		// Fuzzificar as entradas.
		fiService = trapmf(service,6,9,10,10);
		fiFood    = trapmf(food,7,9,10,10);

		// Aplica��o dos operadores Fuzzy.
		fop_rule3 = max_val(fiService,fiFood);

		// Aplica��o do M�todo de Implica��o (valores m�nimos).
		x=0;
		y=0;
		for (int a=0; a<=30; a++)
		{
			y = trimf(x,20,25,30);

			if (y >= fop_rule3)
			{
				tip_gorgeous.at(a) = fop_rule3;
			}
			else
			{
				tip_gorgeous.at(a) = y;
			}

			x=x+1;
		}
	}


	// Aplica��o do M�todo de Agrega��o.
	for (int a=0; a<=30; a++)
	{
		if (a >= 0 && a <= 10)
		{
			tip.at(a) = tip_cheap.at(a);
		}

		if (a >= 10 && a <= 20)
		{
			tip.at(a) = tip_average.at(a);
		}

		if (a >= 20 && a <= 30)
		{
			tip.at(a) = tip_gorgeous.at(a);
		}
	}


	// Implica��o dos antecedentes pelo consequente.
	x = 0;
	total_area = 0;
	sum = 0;
	for (int a=0; a<=30; a++)
	{
		total_area = total_area + tip.at(a);
		sum = sum + (x * tip.at(a));

		x=x+1;
	}

    // C�lculo da Centr�ide.
	gorjeta = sum/total_area;
	FmFuzzy->Label1->Caption = FloatToStrF(gorjeta,ffFixed,10,2);
	posicao_do_grafico = gorjeta;

	for (int a=0; a<=30; a++)
	{
		FmFuzzy->Chart4->Series[0]->YValues->Value[a] = tip.at(a);
	}

	FmFuzzy->Chart3->Refresh();
	FmFuzzy->Chart4->Refresh();
}


//---------------------------------------------------------------------------

__fastcall TFmFuzzy::TFmFuzzy(TComponent* Owner)
	: TForm(Owner)
{




}
//---------------------------------------------------------------------------
void __fastcall TFmFuzzy::FormCreate(TObject *Sender)
{
	// Expande o gr�fico para comportar a quantidade de amostras contidas em max_tela.
	for (unsigned int a = 0; a <= 10; a++)
	{
		Chart1->Series[0]->AddY(0);
		Chart1->Series[1]->AddY(0);
		Chart1->Series[2]->AddY(0);
		Chart2->Series[0]->AddY(0);
		Chart2->Series[1]->AddY(0);
	}
	for (unsigned int a = 0; a <= 30; a++)
	{
		Chart3->Series[0]->AddY(0);
		Chart3->Series[1]->AddY(0);
		Chart3->Series[2]->AddY(0);
		Chart4->Series[0]->AddY(0);
	}

	// Atualiza o chart.
	Chart1->Refresh();
	Chart2->Refresh();
	Chart3->Refresh();
	Chart4->Refresh();

	x=0;
	for (int a=0; a<=10; a++)
	{
		Chart1->Series[0]->YValues->Value[a] = trapmf(x,-1,0,1,4);
		Chart1->Series[1]->YValues->Value[a] = trimf(x,1,5,9);
		Chart1->Series[2]->YValues->Value[a] = trapmf(x,6,9,10,10);
		x=x+1;
	}

	x=0;
	for (int a=0; a<=10; a++)
	{
		Chart2->Series[0]->YValues->Value[a] = trapmf(x,-1,0,1,3);
		Chart2->Series[1]->YValues->Value[a] = trapmf(x,7,9,10,10);
		x=x+1;
	}

	x=0;
	for (int a=0; a<=30; a++)
	{
		Chart3->Series[0]->YValues->Value[a] = trimf(x,0,5,10);
		Chart3->Series[1]->YValues->Value[a] = trimf(x,10,15,20);
		Chart3->Series[2]->YValues->Value[a] = trimf(x,20,25,30);
		x=x+1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFmFuzzy::tbServiceChange(TObject *Sender)
{
	service = tbService->Position;
	posicao_do_grafico_service = tbService->Position;
	Label5->Caption = tbService->Position;

	Chart1->Refresh();

	Fuzzy();
}
//---------------------------------------------------------------------------
void __fastcall TFmFuzzy::tbFoodChange(TObject *Sender)
{
	food = tbFood->Position;
	posicao_do_grafico_food = tbFood->Position;
	Label6->Caption = tbFood->Position;

	Chart2->Refresh();

	Fuzzy();
}
//---------------------------------------------------------------------------
void __fastcall TFmFuzzy::Chart4AfterDraw(TObject *Sender)
{
	double xi, xf, yi, yf;

	xi=Chart4->BottomAxis->CalcPosValue(posicao_do_grafico);
	yi=Chart4->LeftAxis->CalcPosValue(Chart4->LeftAxis->Minimum)+5;
	xf=Chart4->BottomAxis->CalcPosValue(posicao_do_grafico)+1;
	yf=Chart4->LeftAxis->CalcPosValue(Chart4->LeftAxis->Maximum)-5;
	Chart4->Canvas->Pen->Style=psSolid;
	Chart4->Canvas->Pen->Color=clBlack;
	Chart4->Canvas->Rectangle(xi,yi,xf,yf);
}
//---------------------------------------------------------------------------
void __fastcall TFmFuzzy::Chart1AfterDraw(TObject *Sender)
{
	double xi, xf, yi, yf;

	xi=Chart1->BottomAxis->CalcPosValue(posicao_do_grafico_service);
	yi=Chart1->LeftAxis->CalcPosValue(Chart1->LeftAxis->Minimum)+5;
	xf=Chart1->BottomAxis->CalcPosValue(posicao_do_grafico_service)+1;
	yf=Chart1->LeftAxis->CalcPosValue(Chart1->LeftAxis->Maximum)-5;
	Chart1->Canvas->Pen->Style=psSolid;
	Chart1->Canvas->Pen->Color=clBlack;
	Chart1->Canvas->Rectangle(xi,yi,xf,yf);
}
//---------------------------------------------------------------------------

void __fastcall TFmFuzzy::Chart2AfterDraw(TObject *Sender)
{
	double xi, xf, yi, yf;

	xi=Chart2->BottomAxis->CalcPosValue(posicao_do_grafico_food);
	yi=Chart2->LeftAxis->CalcPosValue(Chart2->LeftAxis->Minimum)+5;
	xf=Chart2->BottomAxis->CalcPosValue(posicao_do_grafico_food)+1;
	yf=Chart2->LeftAxis->CalcPosValue(Chart2->LeftAxis->Maximum)-5;
	Chart2->Canvas->Pen->Style=psSolid;
	Chart2->Canvas->Pen->Color=clBlack;
	Chart2->Canvas->Rectangle(xi,yi,xf,yf);
}
//---------------------------------------------------------------------------

void __fastcall TFmFuzzy::Chart3AfterDraw(TObject *Sender)
{
	double xi, xf, yi, yf;

	xi=Chart3->BottomAxis->CalcPosValue(posicao_do_grafico);
	yi=Chart3->LeftAxis->CalcPosValue(Chart3->LeftAxis->Minimum)+5;
	xf=Chart3->BottomAxis->CalcPosValue(posicao_do_grafico)+1;
	yf=Chart3->LeftAxis->CalcPosValue(Chart3->LeftAxis->Maximum)-5;
	Chart3->Canvas->Pen->Style=psSolid;
	Chart3->Canvas->Pen->Color=clBlack;
	Chart3->Canvas->Rectangle(xi,yi,xf,yf);
}
//---------------------------------------------------------------------------

