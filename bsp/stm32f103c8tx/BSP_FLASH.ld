/*
******************************************************************************
**

**  File        : LinkerScript.ld
**
**  Author		: STM32CubeMX
**
**  Abstract    : Linker script for STM32F030CCTx series
**                256Kbytes FLASH and 32Kbytes RAM
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

_FLASH_LENGTH = 64K;
_FLASH_ORIGIN = 0x8000000;
_RAM_LENGTH = 20K;
_RAM_ORIGIN = 0x20000000;
_SYSMEM_ORIGIN = 0x1FFF0000;
_PAGE_LENGTH = 1K;

/************************************
***** Memory layout *****************
------------------------- 0
BL
------------------------- 7K
BL Metadata
------------------------- 8K
APP
------------------------- 63K
APP Metadata
------------------------- 64K
*************************************
*/

/* aligned to page for simplicity */
_LENGTH_META = 1K;
_LENGTH_BL = 7K;
/* Rest of flash */
_LENGTH_APP = _FLASH_LENGTH - (_LENGTH_META + _LENGTH_BL + _LENGTH_META);

_ORIGIN_BL = _FLASH_ORIGIN;
_ORIGIN_BL_META = (_ORIGIN_BL + _LENGTH_BL);
_ORIGIN_APP = _ORIGIN_BL + _LENGTH_BL + _LENGTH_META;
_ORIGIN_APP_META = _ORIGIN_APP + _LENGTH_APP;

/* Should track the size of the reset structure */
_LENGTH_RAM_NOLOAD = 12;
_LENGTH_RAM_LOAD = (_RAM_LENGTH- _LENGTH_RAM_NOLOAD);
_ORIGIN_RAM_NOLOAD = _RAM_ORIGIN + _LENGTH_RAM_LOAD;

/* Specify the memory areas */
MEMORY
{
RAM (xrw)           : ORIGIN = _RAM_ORIGIN, LENGTH = _LENGTH_RAM_LOAD
RAM_NOLOAD (rw)     : ORIGIN = _ORIGIN_RAM_NOLOAD, LENGTH = _LENGTH_RAM_NOLOAD
FLASH_BL (rx)       : ORIGIN = _ORIGIN_BL, LENGTH = _LENGTH_BL
FLASH_BL_META (rw)  : ORIGIN = _ORIGIN_BL_META, LENGTH = _LENGTH_META
/* Ensure the vector table is aligned */
FLASH_APP (rx)      : ORIGIN = _ORIGIN_APP, LENGTH = _LENGTH_APP
FLASH_APP_META (rw) : ORIGIN = _ORIGIN_APP_META, LENGTH = _LENGTH_APP
}
