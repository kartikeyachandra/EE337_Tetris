#include <at89c5131.h>
#include "lcd.h"		//Header file with LCD interfacing functions
#include "serial.c"	//C file with UART interfacing functions

//Variable Decleration
int max=0;
int type_til=0;
int type=0;
int a=192; //0xC0
int b=128; //0x80
int x_post=15;
int y_post=0;
int check=0;
unsigned int score=0;
int fill0=-1;
int fill1=-1;
int temp=0;
int temp1=0;
int gen_lfsr=14;
unsigned char char_c;

//Variable Decl.
bit check_fill(void);
void lfsr_generate(void);
void up(void);
void down(void);
void rotate(void);
void gen_newtil(void);
void update_lcd(void);
void Timer0_init(void);

//Generation of new tile from LFSR
void gen_newtil(void)
{
    lfsr_generate();
    type_til = gen_lfsr%3;
    type = gen_lfsr%2;
    x_post = 15;
    y_post = gen_lfsr%2;
}

//Timer0_init
void Timer0_init(void)
{
	TMOD=TMOD+1;
	TH0 = 0x0;          
  TL0 = 0x01;
	TR0=1;
	EA=1;
}

//Check how much board is Fill
bit check_fill(void)
{
    if(fill1>13||fill0>13)
    {
        return 1;
    }
    return 0;
}
	
//LFSR_Generation
void lfsr_generate(void)
{  
    temp = (gen_lfsr&0x8);
    temp1 = ((gen_lfsr&0x1)<<3);
    gen_lfsr = (gen_lfsr>>1);
    gen_lfsr = gen_lfsr+(temp^temp1);
}

//UP
void up(void)
{
    if(y_post%2 == 0)
        y_post = y_post + 1;
    check=1;
    update_lcd();
}
//Rotate
void rotate(void)
{
	if (type_til != 0)
	{
		if(y_post%2 == 0)
        y_post = y_post + 1;
    check=1;
		type = type + 1;
		
	}
}	


//Down
void down(void)
{
    if(y_post%2 == 1)
        y_post = y_post - 1;
    check=1;
    update_lcd();
}


//Main function to update LCD after every iteration
void update_lcd(void)
{
	if (check_fill())
	{
				if(max<=score)
				{
					max = score;
					score=0;
				}
				lcd_cmd(0x01);   
        msdelay(4);
        lcd_cmd(0x80);
        msdelay(4);
        lcd_write_string("Score:");
        msdelay(4);
        int_to_string(score, &char_c);
        lcd_write_string(&char_c);
        msdelay(4);
        lcd_cmd(0xC0);
        msdelay(4);
        lcd_write_string("max:");
        msdelay(4);
				int_to_string(max, &char_c);
        lcd_write_string(&char_c);
				msdelay(3000);
				lcd_cmd(0x01);
				fill0 = -1;
				fill1 = -1;
	}
	//Boundary Condn.
	else if ( (type_til == 0 && y_post== 1 && fill1 == x_post)|| (type_til == 0 && y_post == 0 && fill0 == x_post) ||(x_post == -1)|| (type_til == 1 && type%2 == 0 && y_post == 0 && fill0 == x_post) || (type_til == 1 && type%2 == 1 &&(fill0 == x_post || fill1 == x_post)) || (type_til == 2 && (type % 4 == 0 || type % 4 == 3) && (fill0 == x_post || fill1 == x_post)) || (type_til == 2 && type % 4 == 1 && (fill1 == x_post || fill0 == x_post + 1)) || (type_til == 2 && type % 4 == 2 && (fill1 == x_post + 1 || fill0 == x_post)))
	{
		if (type_til == 0)
		{
			if (y_post == 0)
			{
				fill0 = fill0 + 1;
			}
			else 
			{
				fill1 = fill1 + 1;
			}
		}
		else if (type_til == 1)
		{
			if (type%2 == 0)
			{
				if (y_post == 0)
				{
					fill0 = fill0 + 2;
				}
				else 
				{
					fill1 = fill1 + 2;
				}
			}
			else
			{
				fill0++;
				fill1++;
				if (fill1 < fill0)
				{
					fill1 = fill0;
				}
				else
				{
					fill0 = fill1;
				}
				
			}
		}
		else
		{
			if (type%4 == 0)
			{
				fill0 = fill0 + 1;
				fill1 = fill1 + 2;
			}
			else if (type%4 == 1)
			{
				if (x_post == fill0)
				{
					fill0 = fill0 + 1;
					fill1 = fill1 + 2;
				}
				else
				{
					fill0 = fill0 + 2;
					fill1 = fill1 + 2;
				}
			}
			else if (type%4 == 2)
			{
				if (x_post == fill1)
				{
					fill0 = fill0 + 2;
					fill1 = fill1 + 1;
				}
				else
				{
					fill0 = fill0 + 2;
					fill1 = fill1 + 2;
				}
			}
			else
			{
				fill0 = fill0 + 2;
				fill1 = fill1 + 1;
			}
		}
		gen_newtil();
	}
	else
	{
		lcd_cmd(a + x_post + 1);
		msdelay(4);
		lcd_write_string("  ");
		msdelay(4);
		lcd_cmd(b + x_post + 1);
		msdelay(4);
		lcd_write_string("  ");
		if(type_til == 0)
		{
			if (y_post == 0)
			{
				lcd_cmd(a + x_post);
				msdelay(4);
				lcd_write_string("*");
				msdelay(4);
			}
			else
			{
				lcd_cmd(b + x_post);
				msdelay(4);
				lcd_write_string("*");
				msdelay(4);
			}
		}
		else if(type_til == 1)
		{
			if (type%2 == 0)
			{
				if (y_post == 0)
				{
					lcd_cmd(a + x_post);
					msdelay(4);
					lcd_write_string("**");
					msdelay(4);
				}
				else 
				{
					lcd_cmd(b + x_post);
					msdelay(4);
					lcd_write_string("**");
					msdelay(4);
				}
			}
			// *
			//**
			else     
			{
				lcd_cmd(a + x_post);
				msdelay(4);
				lcd_write_string("*");
				msdelay(4);
				lcd_cmd(b + x_post);
				msdelay(4);
				lcd_write_string("*");
				msdelay(4);
			}
		}
		else
		{
			if (type%4 == 1)
			{
				lcd_cmd(a + x_post);
				msdelay(4);
				lcd_write_string(" *");
				msdelay(4);
				lcd_cmd(b + x_post);
				msdelay(4);
				lcd_write_string("**");
				msdelay(4);
			}
			else if (type%4 == 0)
			{
				lcd_cmd(a + x_post);
				msdelay(4);
				lcd_write_string("*");
				msdelay(4);
				lcd_cmd(b + x_post);
				msdelay(4);
				lcd_write_string("**");
				msdelay(4);
				
			}
			else
			{
				lcd_cmd(a + x_post);
				msdelay(4);
				lcd_write_string("**");
				msdelay(4);
				lcd_cmd(b + x_post);
				msdelay(4);
				lcd_write_string(" *");
				msdelay(4);
			}
		}
	}
}				

//UART call and main Function
void main(void)
{
	unsigned char ch;
	//Call initialization functions
	lcd_init();
	uart_init();

	while(1)
	{
			ch=' ';
		  update_lcd();
			msdelay(1000);
		  score++;
			x_post--;
			if(rx_complete==1)    //To move Tiles left after 1sec delay without any input
			ch = receive_char();

//Switching case for up/down/rotation			
			switch(ch)
			{
				
				case 'a':down();
								 
								 break;
				
				case 'q':up();
								 
								 break;
		
				case 'r':rotate();
								break;
							
				default:break;
								 
				
			}
			msdelay(100);
				
	}			

}