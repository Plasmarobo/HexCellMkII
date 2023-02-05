/*
******************************************************************************
**

**  File        : LinkerScript.ld
**
**  Author		: STM32CubeMX
**
**  Abstract    : Linker script for STM32F103C8Tx series
**                64Kbytes FLASH and 20Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**   1. Redistributions of source code must retain the above copyright notice,
**      this list of conditions and the following disclaimer.
**   2. Redistributions in binary form must reproduce the above copyright notice,
**      this list of conditions and the following disclaimer in the documentation
**      and/or other materials provided with the distribution.
**   3. Neither the name of STMicroelectronics nor the names of its contributors
**      may be used to endorse or promote products derived from this software
**      without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*****************************************************************************
*/

_ORIGIN_FLASH = 0x8000000;
_LENGTH_FLASH = 64K;
_LAST_FLASH = _ORIGIN_FLASH + _LENGTH_FLASH - 1;
_LENGTH_RAM_ALL = 20K;
_ORIGIN_RAM = 0x20000000;
_ORIGIN_SYSMEM = 0x1FFF0000;
_LENGTH_PAGE = 1K;
_LENGTH_META = 16;
_LENGTH_KEYSTORE = _LENGTH_PAGE;

/* Must be aligned to page */
_LENGTH_BL = 32K - _LENGTH_META;
/* Rest of flash */
_LENGTH_APP = _LENGTH_FLASH - ((_LENGTH_BL + _LENGTH_META) + (_LENGTH_META + _LENGTH_KEYSTORE));

_ORIGIN_BL = _ORIGIN_FLASH;
_ORIGIN_BL_META = (_ORIGIN_BL + _LENGTH_BL);
_ORIGIN_APP = (_ORIGIN_BL_META + _LENGTH_META);
_ORIGIN_APP_META = _ORIGIN_APP + _LENGTH_APP;
_ORIGIN_KEYSTORE = _ORIGIN_APP_META + _LENGTH_META;

/* Should track the size of the reset structure */

_LENGTH_RAM_NOLOAD = 12;
_LENGTH_RAM_LOAD = (_LENGTH_RAM_ALL - _LENGTH_RAM_NOLOAD);
_ORIGIN_RAM_NOLOAD = _ORIGIN_RAM + _LENGTH_RAM_LOAD;

/* Highest address of the user mode stack */
_estack = _ORIGIN_RAM + _LENGTH_RAM_ALL;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x0;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
RAM (xrw)           : ORIGIN = _ORIGIN_RAM, LENGTH = _LENGTH_RAM_LOAD
RAM_NOLOAD (rw)     : ORIGIN = _ORIGIN_RAM_NOLOAD, LENGTH = _LENGTH_RAM_NOLOAD
FLASH_BL (rx)       : ORIGIN = _ORIGIN_BL, LENGTH = _LENGTH_BL
FLASH_BL_META (rw)  : ORIGIN = _ORIGIN_BL_META, LENGTH = _LENGTH_META
/* Ensure the vector table is aligned */
FLASH_APP (rx)      : ORIGIN = _ORIGIN_APP, LENGTH = _LENGTH_APP
FLASH_APP_META (rw) : ORIGIN = _ORIGIN_APP_META, LENGTH = _LENGTH_META
FLASH_KEYSTORE (rw) : ORIGIN = _ORIGIN_KEYSTORE, LENGTH = _LENGTH_KEYSTORE
}