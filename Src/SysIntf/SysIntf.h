/*
 * SysIntf.h
 *
 *  Created on: 8. juli 2012
 *      Author: eskelund
 */

#ifndef SYSINTF_H_
#define SYSINTF_H_

#ifdef __cplusplus
 extern "C" {
#endif

 void SysIntf_IRQHandler(void);

 class SYSINTF_t
{
	private:

	public:
	 	 SYSINTF_t();
	 	 static void Interrupt(void);
	 	 int Init(void);
	 	 int Send(char *str, uint16_t size);
};

#ifdef __cplusplus
}
#endif

#endif /* SYSINTF_H_ */
