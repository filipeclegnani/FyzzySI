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


float maximo(float a, float b) {
  if (a > b) {
    return a;
  }
  return b;
}

float minimo(float a, float b) {
  if (a < b) {
    return a;
  }
  return b;
}

float absoluto(float a){
	if (a < 0.0f){
		return -a;
	}
	return a;
}


#define TAMANHO 15
float rBaixo[TAMANHO + 1];
float rMedio[TAMANHO + 1];
float rGrande[TAMANHO + 1];

// Vari�veis Fuzzy.
float erro_atual = 0;
int erro_anterior = 0;
unsigned int setpoint = 7000;
float fiFood    = 0;
float foTip     = 0;
float rule = 0;
unsigned int rpm = 0;
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



      int rpmAux = rpm;


      erro_anterior = erro_atual;
	  erro_atual = minimo(absoluto(setpoint - rpm), 2000);
      int delta = maximo(minimo(abs(erro_atual - erro_anterior), 1000), 1);
      float x = 0;
      float rule = 0;

      // 1� regra - Se a proximidade � alta, ent�o o reajuste � baixo
      if (erro_atual <= 100) {
        // Fuzzificar as entradas e aplica��o dos operadores

        rule = trapmf(erro_atual, -1, 0, 1, 100);

        x = 0;
        for (int a = 0; a <= TAMANHO; a++) {
          rBaixo[a] = trapmf(x, -1, 0, 0, 5);
          if (rBaixo[a] >= rule) {
            rBaixo[a] = rule;
          }

          x += 1;
        }
      }

      // 2� regra - Se a proximidade � m�dia e est� em ajuste baixo, o reajuste
      // � baixo
      if (erro_atual >= 100 && erro_atual <= 1001 && delta <= 200) {
        // Fuzzificar as entradas e aplica��o dos operadores
        rule = trapmf(erro_atual, 50, 800, 800, 1550) *
               (1 - trapmf(delta, -1, 0, 1, 200));

        x = 0;
        for (int a = 0; a <= TAMANHO; a++) {
          rBaixo[a] = trapmf(x, 2, 5, 5, 10);
          if (rBaixo[a] >= rule) {
            rBaixo[a] = rule;
          }

          x += 1;
        }
      }

      // 3� regra - Se a proximidade � m�dia e est� em ajuste alto, o reajuste �
      // m�dio
      if (erro_atual >= 100 && erro_atual <= 1001 && delta >= 201) {
        // Fuzzificar as entradas e aplica��o dos operadores
        rule = trapmf(erro_atual, 50, 800, 800, 1550) *
               trapmf(delta, 200, 500, 2000, 2001);

        x = 0;
        for (int a = 0; a <= TAMANHO; a++) {
          rMedio[a] = trapmf(x, 2, 5, 5, 10);

          if (rMedio[a] >= rule) {
            rMedio[a] = rule;
          }

          x += 1;
        }
      }

      // 4� regra - Se a proximidade � baixa, ent�o o reajuste � alto
      if (erro_atual >= 1001) {
        // Fuzzificar as entradas e aplica��o dos operadores
        rule = trapmf(erro_atual, 1001, 1500, 2000, 2001);

        x = 0;
        for (int a = 0; a <= TAMANHO; a++) {
          rGrande[a] = trapmf(x, 5, 10, 15, 16);

          if (rGrande[a] >= rule) {
            rGrande[a] = rule;
          }

          x += 1;
        }
      }

      // Aplica��o do M�todo de agrega��o e implica��o dos antecedentes pelo
      // consequente.
      x = 0;
	  float total_area = 0;
	  float soma = 0;
	  for (int a = 0; a <= TAMANHO; a++) {
		if (a >= 0 && a <= 5) {
		  total_area += rBaixo[a];
		  soma += (x * rBaixo[a]);
		  FmFuzzy->Chart4->Series[0]->YValues->Value[a] = rBaixo[a];
        }

        if (a >= 5 && a <= 10) {
		  total_area += rMedio[a];
          soma += (x * rMedio[a]);
		  FmFuzzy->Chart4->Series[0]->YValues->Value[a] = rMedio[a];
        }

        if (a >= 10 && a <= TAMANHO) {
		  total_area += rGrande[a];
		  soma += (x * rGrande[a]);
		  FmFuzzy->Chart4->Series[0]->YValues->Value[a] = rGrande[a];
		}

		x += 1;
      }
	  FmFuzzy->Label1->Caption = FloatToStrF(soma/total_area,ffFixed,10,2);
	  // C�lculo da Centr�ide.
      float reajuste = 0;
	  if (total_area != 0) {
		reajuste = soma / total_area;
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
	for (unsigned int a = 0; a <= 9000; a++)
	{
		Chart1->Series[0]->AddY(0);
		Chart1->Series[1]->AddY(0);
		Chart1->Series[2]->AddY(0);
		Chart2->Series[0]->AddY(0);
		Chart2->Series[1]->AddY(0);
	}
	for (unsigned int a = 0; a <= 9000; a++)
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
	for (int a=0; a<=2100; a++)
	{
		Chart1->Series[0]->YValues->Value[a] = trapmf(x, -1, 0, 1, 100);
		Chart1->Series[1]->YValues->Value[a] = trapmf(x, 50, 800, 800, 1550);
		Chart1->Series[2]->YValues->Value[a] = trapmf(x, 1001, 1500, 2000, 2001);
		x=x+1;
	}

	x=0;
	for (int a=0; a<=2100; a++)
	{
		Chart2->Series[0]->YValues->Value[a] = trapmf(x, -1, 0, 1, 200);
		Chart2->Series[1]->YValues->Value[a] = trapmf(x, 200, 500, 2000, 2001);
		x=x+1;
	}

	x=0;
	for (int a=0; a<=15; a++)
	{
		Chart3->Series[0]->YValues->Value[a] = trapmf(x, -1, 0, 0, 5);
		Chart3->Series[1]->YValues->Value[a] = trapmf(x, 2, 5, 5, 10);
		Chart3->Series[2]->YValues->Value[a] = trapmf(x, 5, 10, 15, 16);
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


