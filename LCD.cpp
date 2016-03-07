//--------------------------------------------------------------------------------------------- //
//		AVR e Arduino: Técnicas de Projeto, 2a ed. - 2012.										//
//--------------------------------------------------------------------------------------------- //
//=============================================================================================	//
//		Sub-rotinas para o trabalho com um LCD 16x2 com via de dados de 4 bits					//
//		Controlador HD44780	- Pino R/W aterrado													//
//		A via de dados do LCD deve ser ligado aos 4 bits mais significativos ou					//
//		aos 4 bits menos significativos do PORT do uC											//
//=============================================================================================	//
#include "LCD.h"

//---------------------------------------------------------------------------------------------
// Sub-rotina para enviar caracteres e comandos ao LCD com via de dados de 4 bits
//---------------------------------------------------------------------------------------------
void cmd_LCD(unsigned char c, char cd)				//c é o dado  e cd indica se é instrução ou caractere
{
	if(cd==0)
		clr_bit(CTRL_PORT_LCD,RS);
	else
		set_bit(CTRL_PORT_LCD,RS);

	//primeiro nibble de dados - 4 MSB
	#if (nibble_dados)								//compila código para os pinos de dados do LCD nos 4 MSB do PORT
		DATA_PORT_LCD = (DATA_PORT_LCD & 0x0F)|(0xF0 & c);
	#else											//compila código para os pinos de dados do LCD nos 4 LSB do PORT
		DATA_PORT_LCD = (DATA_PORT_LCD & 0xF0)|(c>>4);
	#endif

	pulso_enable();

	//segundo nibble de dados - 4 LSB
	#if (nibble_dados)								//compila código para os pinos de dados do LCD nos 4 MSB do PORT
		DATA_PORT_LCD = (DATA_PORT_LCD & 0x0F) | (0xF0 & (c<<4));
	#else											//compila código para os pinos de dados do LCD nos 4 LSB do PORT
		DATA_PORT_LCD = (DATA_PORT_LCD & 0xF0) | (0x0F & c);
	#endif

	pulso_enable();

	if((cd==0) && (c<4))				//se for instrução de retorno ou limpeza espera LCD estar pronto
		_delay_ms(2);
}
//---------------------------------------------------------------------------------------------
//Sub-rotina para inicialização do LCD com via de dados de 4 bits
//---------------------------------------------------------------------------------------------
void LCD_Init()		//sequência ditada pelo fabricando do circuito integrado HD44780
{							//o LCD será só escrito. Então, R/W é sempre zero.
	
	// I/O Setup //
	
	CTRL_DDR_LCD |= (1<<E) | (1<<RS);
	
	#if (nibble_dados)
		DATA_DDR_LCD |= 0b11110000; // configure data pins as output
	#else
		DATA_DDR_LCD |= 0b00001111; // configure data pins as output
	#endif
	
	
	clr_bit(CTRL_PORT_LCD,RS);	//RS em zero indicando que o dado para o LCD será uma instrução
	clr_bit(CTRL_PORT_LCD,E);	//pino de habilitação em zero

	_delay_ms(20);	 		//tempo para estabilizar a tensão do LCD, após VCC ultrapassar 4.5 V (na prática pode
							//ser maior).
	//interface de 8 bits
	#if (nibble_dados)
		DATA_PORT_LCD = (DATA_PORT_LCD & 0x0F) | 0x30;
	#else
		DATA_PORT_LCD = (DATA_PORT_LCD & 0xF0) | 0x03;
	#endif

	pulso_enable();			//habilitação respeitando os tempos de resposta do LCD
	_delay_ms(5);
	pulso_enable();
	_delay_us(200);
	pulso_enable();	/*até aqui ainda é uma interface de 8 bits.
					Muitos programadores desprezam os comandos acima, respeitando apenas o tempo de
					estabilização da tensão (geralmente funciona). Se o LCD não for inicializado primeiro no
					modo de 8 bits, haverá problemas se o microcontrolador for inicializado e o display já o tiver sido.*/

	//interface de 4 bits, deve ser enviado duas vezes (a outra está abaixo)
	#if (nibble_dados)
		DATA_PORT_LCD = (DATA_PORT_LCD & 0x0F) | 0x20;
	#else
		DATA_PORT_LCD = (DATA_PORT_LCD & 0xF0) | 0x02;
	#endif

	pulso_enable();
   	cmd_LCD(0x28,0); 		//interface de 4 bits 2 linhas (aqui se habilita as 2 linhas)
							//são enviados os 2 nibbles (0x2 e 0x8)
   	cmd_LCD(0x08,0);		//desliga o display
   	cmd_LCD(0x01,0);		//limpa todo o display
   	cmd_LCD(0x0C,0);		//mensagem aparente cursor inativo não piscando
   	cmd_LCD(0x80,0);		//inicializa cursor na primeira posição a esquerda - 1a linha
	   
	// Intro Msg //
	
	LCD_Write("   ELETRONICA   ");
	LCD_Cursor_Pos(1,0);
	LCD_Write("      IFSC      ");
	_delay_ms(2000);
	
	LCD_CS();
	LCD_Write("  Kit Didatico  ");
	LCD_Cursor_Pos(1,0);
	LCD_Write("Fotovoltaico M.2");
	_delay_ms(2000);
	
	   
}
//---------------------------------------------------------------------------------------------
//Sub-rotina de escrita no LCD -  dados armazenados na RAM
//---------------------------------------------------------------------------------------------
void LCD_Write(const char *c)
{
   for (; *c!=0;c++) cmd_LCD(*c,1);
}
//---------------------------------------------------------------------------------------------
//Sub-rotina de escrita no LCD - dados armazenados na FLASH
//---------------------------------------------------------------------------------------------

//Conversão de um número em seus digitos individuais
//---------------------------------------------------------------------------------------------
void LCD_ident_num(unsigned int valor,char *disp)
{
 	unsigned char n;

	for(n=0; n<tam_vetor+1; n++)
		disp[n] = '\0'; // 0 + conv_ascii;		//limpa vetor para armazenagem do digitos
		
	char aux[5];
	
	int x = 0;
	do
	{
       aux[x] = (valor%10) + conv_ascii;	//pega o resto da divisao por 10
	   valor /=10;						//pega o inteiro da divisão por 10
	   x++;

	}while (valor!=0);
	
	x--;
	
	while(x>=0)
	{
		*disp = aux[x];
		disp++;
		x--;
	}
}
//---------------------------------------------------------------------------------------------
//Limpa o display e retorna o cursor para o inicio
//---------------------------------------------------------------------------------------------
void LCD_CS()
{
	cmd_LCD(0x01,0);
	cmd_LCD(0x80,0);		//inicializa cursor na primeira posição a esquerda - 1a linha
}
//---------------------------------------------------------------------------------------------
//Move o cursor para a posição desejada
//---------------------------------------------------------------------------------------------
void LCD_Cursor_Pos(int linha,int coluna)
{
	if(linha>1||coluna>15)
		return;
		
	unsigned char cmd;
	
	if(!linha)
	{	
		cmd = 0x80;
		cmd |= coluna;
		cmd_LCD(cmd,0);	
	}
	else
	{
		cmd = 0xC0;
		cmd |= coluna;
		cmd_LCD(cmd,0);
	}	

}

void LCD_Refresh(uint8_t state)
{
	switch(state)
	{
		default:
		return;
	}
}
