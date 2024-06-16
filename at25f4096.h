// @File		AT25F4096.H		 		
// @Author		JOSIMAR PEREIRA LEITE
// @country		Brazil
// @Date		26/03/24
//
//
// Copyright (C) 2024  JOSIMAR PEREIRA LEITE
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
#ifndef AT25F4096_H
#define AT25F4096_H

#define AT25F4096_SCK PORTCbits.RC7
#define AT25F4096_SDI PORTCbits.RC6
#define AT25F4096_SDO PORTCbits.RC5
#define AT25F4096_CS PORTCbits.RC4

#define AT25F4096_SCK_TRIS TRISCbits.RC7
#define AT25F4096_SDI_TRIS TRISCbits.RC6
#define AT25F4096_SDO_TRIS TRISCbits.RC5
#define AT25F4096_CS_TRIS TRISCbits.RC4
//----------------------------------------------
//
//
//
#define AT25F4096_COMMAND_WRITE_ENABLE 0x06
#define AT25F4096_COMMAND_WRITE_DISABLE 0x04
#define AT25F4096_COMMAND_READ_STATUS 0x05
#define AT25F4096_COMMAND_WRITE_STATUS 0x01
#define AT25F4096_COMMAND_READ_DATA 0x03
#define AT25F4096_COMMAND_PAGE_PROGRAM 0x02
#define AT25F4096_COMMAND_SETOR_ERASE 0x52
#define AT25F4096_COMMAND_CHIP_ERASE 0x62
#define AT25F4096_COMMAND_READ_INDENTIFICATION 0x15

static void SPI_write(unsigned char data)
{
    for(unsigned char mask = 0x80; mask; mask >>= 1)
    {            
        if(data & mask) AT25F4096_SDI = 1; else AT25F4096_SDI = 0;  
        
        __asm__ __volatile__("nop"); 
        AT25F4096_SCK = 1;
        __asm__ __volatile__("nop");        
        AT25F4096_SCK = 0;    
        __asm__ __volatile__("nop");
    }    
}
//----------------------------------------------
//
//
//
static unsigned char SPI_read(void)
{
    unsigned char data = 0;
    
    for(unsigned char mask = 0x80; mask; mask >>=1)
    {
        data <<= 1;
        if(AT25F4096_SDO) data |= 1;
        
        __asm__ __volatile__("nop");
        AT25F4096_SCK = 1;
        __asm__ __volatile__("nop");
        AT25F4096_SCK = 0;        
        __asm__ __volatile__("nop");
    }
    
    return ((unsigned char) data);
}
//----------------------------------------------
//
//
//
void AT25F4096_WREN(void)
{
    AT25F4096_CS = 0;    
    SPI_write(AT25F4096_COMMAND_WRITE_ENABLE);
    AT25F4096_CS = 1;
}
//----------------------------------------------
//
//
//
void AT25F4096_WRDI(void)
{
    AT25F4096_CS = 0;    
    SPI_write(AT25F4096_COMMAND_WRITE_DISABLE);
    AT25F4096_CS = 1;
}
//----------------------------------------------
//
//
//
unsigned char AT25F4096_RDSR(void)
{                     
    unsigned char data = 0;
    
    AT25F4096_CS = 0;    
    SPI_write(AT25F4096_COMMAND_READ_STATUS);
    data = SPI_read();
    AT25F4096_CS = 1;        
    return ((unsigned char) data);
}
//----------------------------------------------
//
//
//
void AT25F4096_CHIP_ERASE(void)
{
    AT25F4096_WREN();
    
    AT25F4096_CS = 0;
    SPI_write(AT25F4096_COMMAND_CHIP_ERASE);
    AT25F4096_CS = 1;
    
    AT25F4096_WRDI();
    
    // CHECK BUSY
    unsigned char status = 1;
    while((status & 0x01) == 1) status = AT25F4096_RDSR();
}
//----------------------------------------------
//
//
//
void AT25F4096_SECTOR_ERASE(unsigned long address)
{
    AT25F4096_WREN();
    
    AT25F4096_CS = 0;
    SPI_write(AT25F4096_COMMAND_SETOR_ERASE); 
    SPI_write((unsigned char)((address >> 16) & 0xFF));
    SPI_write((unsigned char)((address >> 8) & 0xFF));
    SPI_write((unsigned char)(address & 0xFF));
    AT25F4096_CS = 1;
    
    AT25F4096_WRDI();
    
    // CHECK BUSY
    unsigned char status = 1;
    while((status & 0x01) == 1) status = AT25F4096_RDSR();
}
//----------------------------------------------
//
//
//
void AT25F4096_WRSR(unsigned char data)
{
    AT25F4096_WREN();
    
    AT25F4096_CS = 0;
    SPI_write(AT25F4096_COMMAND_WRITE_STATUS);
    SPI_write((unsigned char)data);
    AT25F4096_CS = 1;
    
    AT25F4096_WRDI();
    
    // CHECK BUSY
    unsigned char status = 1;
    while((status & 0x01) == 1) status = AT25F4096_RDSR();
}
//----------------------------------------------
//
//
//
// DEVICE ID 2 BYTE; MANUFACTURER: C2H, MEMORY TYPE: 20H
// DEVICE INDIVIDUAL  13H for IDAT25F4096
void AT25F4096_RDID(unsigned char *manufacturer,
unsigned char *device_code)
{                 
    AT25F4096_WRDI();
    
    AT25F4096_CS = 0;    
    SPI_write(AT25F4096_COMMAND_READ_INDENTIFICATION);
    *manufacturer = SPI_read();
    *device_code = SPI_read();
    AT25F4096_CS = 1;
}
//----------------------------------------------
//
//
//
unsigned char AT25F4096_READ_BYTE(unsigned long address)
{         
    unsigned char byte = 0;
    
    AT25F4096_CS = 0;    
    SPI_write(AT25F4096_COMMAND_READ_DATA);
    SPI_write((unsigned char)((address >> 16) & 0xFF));
    SPI_write((unsigned char)((address >> 8) & 0xFF));
    SPI_write((unsigned char)(address & 0xFF));
    byte = SPI_read();
    AT25F4096_CS = 1;
    
    // CHECK BUSY
    unsigned char status = 1;
    while((status & 0x01) == 1) status = AT25F4096_RDSR();
    
    return ((unsigned char) byte);        
}
//----------------------------------------------
//
//
//
void AT25F4096_READ_BUF(unsigned long address, int size, unsigned char buf[])
{                     
    AT25F4096_CS = 0;    
    SPI_write(AT25F4096_COMMAND_READ_DATA);    
    
    SPI_write((unsigned char)((address >> 16) & 0xFF));
    SPI_write((unsigned char)((address >> 8) & 0xFF));
    SPI_write((unsigned char)(address & 0xFF));

    for(int i = 0; i < size; i++)
    {            
        buf[i] = SPI_read(); 
    }    
    AT25F4096_CS = 1;
    
    // CHECK BUSY
    unsigned char status = 1;
    while((status & 0x01) == 1) status = AT25F4096_RDSR();
}
//----------------------------------------------
//
//
//
void AT25F4096_PROGRAM_BUF(unsigned long address, int size, unsigned char buf[])
{
    AT25F4096_WREN();
        
    AT25F4096_CS = 0;     
    SPI_write(AT25F4096_COMMAND_PAGE_PROGRAM);    
    
    SPI_write((unsigned char)((address >> 16) & 0xFF));
    SPI_write((unsigned char)((address >> 8) & 0xFF));
    SPI_write((unsigned char)(address & 0xFF));

    for(int i = 0; i < size; i++)
    {                
        SPI_write(buf[i]);
    }    
    AT25F4096_CS = 1; 
    
    AT25F4096_WRDI();
    
    // CHECK BUSY
    unsigned char status = 1;
    while((status & 0x01) == 1) status = AT25F4096_RDSR();    
}

//----------------------------------------------
//
//
//
void AT25F4096_Init(void)
{
    AT25F4096_SCK_TRIS = 0;
    AT25F4096_SDI_TRIS = 0;
    AT25F4096_SDO_TRIS = 1;
    AT25F4096_CS_TRIS = 0;
    
    AT25F4096_CS = 1;
}

#endif 