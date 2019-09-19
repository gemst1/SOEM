/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

#include "ethercat.h"
#include "pdo_def.h"
#include "servo_def.h"

#define EC_TIMEOUTMON 500
#define NUMOFEPOS4_DRIVE	1

EPOS4_Drive_pt	epos4_drive_pt[NUMOFEPOS4_DRIVE];
int started[NUMOFEPOS4_DRIVE]={0}, ServoState=0;


char IOmap[4096];
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;

int os;
uint32_t ob;
uint16_t ob2;
uint8_t  ob3;

int ServoOn_GetCtrlWrd(uint16_t StatusWord, uint16_t *ControlWord)
{
	int  _enable=0;
	if (bit_is_clear(StatusWord, STATUSWORD_OPERATION_ENABLE_BIT)) //Not ENABLED yet
	{
		if (bit_is_clear(StatusWord, STATUSWORD_SWITCHED_ON_BIT)) //Not SWITCHED ON yet
		{
			if (bit_is_clear(StatusWord, STATUSWORD_READY_TO_SWITCH_ON_BIT)) //Not READY to SWITCH ON yet
			{
				if (bit_is_set(StatusWord, STATUSWORD_FAULT_BIT)) //FAULT exist
				{
					(*ControlWord)=0x80;	//FAULT RESET command
				}
				else //NO FAULT
				{
					(*ControlWord)=0x06;	//SHUTDOWN command (transition#2)
				}
			}
			else //READY to SWITCH ON
			{
				(*ControlWord)=0x07;	//SWITCH ON command (transition#3)
			}
		}
		else //has been SWITCHED ON
		{
			(*ControlWord)=0x0F;	//ENABLE OPETATION command (transition#4)
			_enable=1;
		}
	}
	else //has been ENABLED
	{
		(*ControlWord)=0x0F;	//maintain OPETATION state
		_enable=1;
	}
	return _enable;;
}

void simpletest(char *ifname)
{
    int i, k, oloop, iloop, chk, wkc_count, p1;
    needlf = FALSE;
    inOP = FALSE;

   printf("Starting simple test\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */


       if ( ec_config_init(FALSE) > 0 )
      {
         printf("%d slaves found and configured.\n",ec_slavecount);
		 for (k=0; k<NUMOFEPOS4_DRIVE; ++k)
		 {
			 if (( ec_slavecount >= 1 ) && (strcmp(ec_slave[k+1].name,"EPOS4") == 0)) //change name for other drives
			 {
				/*os=sizeof(ob2); ob2 = 0x0080;
				wkc_count=ec_SDOwrite(k+1, 0x6040, 0x00, FALSE, os, &ob2, EC_TIMEOUTRXM);
				sleep(1);

				os=sizeof(ob2); ob2 = 0x0000;
				wkc_count=ec_SDOwrite(k+1, 0x6040, 0x00, FALSE, os, &ob2, EC_TIMEOUTRXM);
				sleep(1);

				os=sizeof(ob2); ob2 = 0x0006;
				wkc_count=ec_SDOwrite(k+1, 0x6040, 0x00, FALSE, os, &ob2, EC_TIMEOUTRXM);
				sleep(1);

				os=sizeof(ob2); ob2 = 0x0007;
				wkc_count=ec_SDOwrite(k+1, 0x6040, 0x00, FALSE, os, &ob2, EC_TIMEOUTRXM);
				sleep(1);

				os=sizeof(ob2); ob2 = 0x000F;
				wkc_count=ec_SDOwrite(k+1, 0x6040, 0x00, FALSE, os, &ob2, EC_TIMEOUTRXM);
				sleep(1);*/

				printf("Re mapping for EPOS4...\n");
				os=sizeof(ob); ob = 0x00;	//RxPDO, check MAXPOS ESI
				//0x1c12 is Index of Sync Manager 2 PDO Assignment (output RxPDO), CA (Complete Access) must be TRUE
				wkc_count=ec_SDOwrite(k+1, 0x1c12, 0x00, FALSE, os, &ob, EC_TIMEOUTRXM);
				wkc_count=ec_SDOwrite(k+1, 0x1c13, 0x00, FALSE, os, &ob, EC_TIMEOUTRXM);

				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1A00, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				os=sizeof(ob); ob = 0x60410010;
				wkc_count=ec_SDOwrite(k+1, 0x1A00, 0x01, FALSE, os, &ob, EC_TIMEOUTRXM);
				os=sizeof(ob); ob = 0x60640020;
				wkc_count=ec_SDOwrite(k+1, 0x1A00, 0x02, FALSE, os, &ob, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x02;
				wkc_count=ec_SDOwrite(k+1, 0x1A00, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				 if (wkc_count==0)
				 {
					 printf("RxPDO assignment error\n");
					 //return FALSE;
				 }
				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1A01, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1A02, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1A03, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				
				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1600, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				os=sizeof(ob); ob = 0x60400010;
				wkc_count=ec_SDOwrite(k+1, 0x1600, 0x01, FALSE, os, &ob, EC_TIMEOUTRXM);
				os=sizeof(ob); ob = 0x607A0020;
				wkc_count=ec_SDOwrite(k+1, 0x1600, 0x02, FALSE, os, &ob, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x02;
				wkc_count=ec_SDOwrite(k+1, 0x1600, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				 if (wkc_count==0)
				 {
					 printf("TxPDO assignment error\n");
					 //return FALSE;
				 }

				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1601, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1602, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x00;
				wkc_count=ec_SDOwrite(k+1, 0x1603, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);

				os=sizeof(ob2); ob2 = 0x1600;
				wkc_count=ec_SDOwrite(k+1, 0x1C12, 0x01, FALSE, os, &ob2, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x01;
				wkc_count=ec_SDOwrite(k+1, 0x1C12, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);

				os=sizeof(ob2); ob2 = 0x1A00;
				wkc_count=ec_SDOwrite(k+1, 0x1C13, 0x01, FALSE, os, &ob2, EC_TIMEOUTRXM);
				os=sizeof(ob3); ob3 = 0x01;
				wkc_count=ec_SDOwrite(k+1, 0x1C13, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);

				os=sizeof(ob3); ob3 = 0x01;
				wkc_count=ec_SDOwrite(k+1, 0x60C2, 0x01, FALSE, os, &ob3, EC_TIMEOUTRXM);

				os=sizeof(ob3); ob3 = 0x08;
				wkc_count=ec_SDOwrite(k+1, 0x6060, 0x00, FALSE, os, &ob3, EC_TIMEOUTRXM);

				os=sizeof(ob); ob = 0x000186A0;
				wkc_count=ec_SDOwrite(k+1, 0x6065, 0x00, FALSE, os, &ob, EC_TIMEOUTRXM);
			 }
		 }


         ec_config_map(&IOmap);

         ec_configdc();

         //printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         //ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         //if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         //if (iloop > 8) iloop = 8;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 200;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }
         while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");

			//wkc_count = 0;

			for (k=0; k<NUMOFEPOS4_DRIVE; ++k)
			{
				epos4_drive_pt[k].ptOutParam=(EPOS4_DRIVE_RxPDO_t*)  ec_slave[k+1].outputs;
				epos4_drive_pt[k].ptInParam= (EPOS4_DRIVE_TxPDO_t*)  ec_slave[k+1].inputs;
				
			}
		   
			ec_dcsync0(1, TRUE, 1000000, 0);
			ec_dcsync0(2, TRUE, 1000000, 0);

		   for (i=0; i<NUMOFEPOS4_DRIVE; ++i)
		   {
			   epos4_drive_pt[i].ptOutParam->ControlWord = 0x06;
				//*(ec_slave[i+1].outputs) = 0x06000000;
		   }
           ec_send_processdata();
           wkc = ec_receive_processdata(EC_TIMEOUTRET);
		   printf("Shutdown EPOS4 \n");
		   osal_usleep(800);

		   for (i=0; i<NUMOFEPOS4_DRIVE; ++i)
		   {
			   epos4_drive_pt[i].ptOutParam->ControlWord = 0x0F;
				//*(ec_slave[i+1].outputs) = 0x0F000000;
		   }
           ec_send_processdata();
           wkc = ec_receive_processdata(EC_TIMEOUTRET);
		   //sleep(1);
		   printf("Status word 0 = 0x%X\n", epos4_drive_pt[0].ptInParam->StatusWord);
//		   printf("Status word 1 = 0x%X\n", epos4_drive_pt[1].ptInParam->StatusWord);
		   p1 = epos4_drive_pt[0].ptInParam->PositionActualValue;
//		   p2 = epos4_drive_pt[1].ptInParam->PositionActualValue;
		   printf("Present Position 0 = %i\n", p1);
//		   printf("Present Position 1 = %i\n", p2);
		   printf("Switch on EPOS4 \n");
			osal_usleep(800);

		   epos4_drive_pt[0].ptOutParam->TargetPosition = p1;
//		   epos4_drive_pt[1].ptOutParam->TargetPosition = p2;
		   ec_send_processdata();
           wkc = ec_receive_processdata(EC_TIMEOUTRET);
		   osal_usleep(800);
			inOP = TRUE;

			/*for(i = 1; i <= 100; i++)
            {
               ec_send_processdata();
			   wkc = ec_receive_processdata(EC_TIMEOUTRET);
			   //sleep(1);
			   printf("Status word = 0x%X\n", );
			   osal_usleep(5000);

            }*/
			do
		     {
		        //epos4_drive_pt[0].ptOutParam->ControlWord = 0x0F;
		        //epos4_drive_pt[1].ptOutParam->ControlWord = 0x0F;
				ec_send_processdata();
			    wkc = ec_receive_processdata(EC_TIMEOUTRET);
			    printf("Status word 0 = 0x%X\n", epos4_drive_pt[0].ptInParam->StatusWord);
//	  		    printf("Status word 1 = 0x%X\n", epos4_drive_pt[1].ptInParam->StatusWord);
				printf("Position 0 = %i / %i\n", epos4_drive_pt[0].ptInParam->PositionActualValue, epos4_drive_pt[0].ptOutParam->TargetPosition);				
//				printf("Position 1 = %i / %i\n", epos4_drive_pt[1].ptInParam->PositionActualValue, epos4_drive_pt[1].ptOutParam->TargetPosition);
				osal_usleep(800);
		     }
//         	while (epos4_drive_pt[0].ptInParam->StatusWord != 0x1237 || epos4_drive_pt[1].ptInParam->StatusWord != 0x1237);
             while (epos4_drive_pt[0].ptInParam->StatusWord != 0x1237);


                /* cyclic loop */
             while (epos4_drive_pt[0].ptInParam->StatusWord == 0x1237) {
                 for (i = 1; i <= 5000; i++) {
                     /*ec_send_processdata();
                     wkc = ec_receive_processdata(EC_TIMEOUTRET);

                     if(wkc >= expectedWKC)
                     {
                         printf("Processdata cycle %4d, WKC %d , O:", i, wkc);

                         for(j = 0 ; j < oloop; j++)
                         {
                             printf(" %2.2x", *(ec_slave[0].outputs + j));
                         }

                         printf(" I:");
                         for(j = 0 ; j < iloop; j++)
                         {
                             printf(" %2.2x", *(ec_slave[0].inputs + j));
                         }
                         printf(" T:%"PRId64"\r",ec_DCtime);
                         needlf = TRUE;
                     }*/
                     //epos4_drive_pt[0].ptOutParam->ControlWord = 0x0F;
                     epos4_drive_pt[0].ptOutParam->TargetPosition = i * 50 + p1;
//				epos4_drive_pt[1].ptOutParam->TargetPosition = -i*50+p2;
                     ec_send_processdata();
                     wkc = ec_receive_processdata(EC_TIMEOUTRET);
                     printf("Status word 0 = 0x%X\n", epos4_drive_pt[0].ptInParam->StatusWord);
                     if(epos4_drive_pt[0].ptInParam->StatusWord != 0x1237)break;
//				printf("Status word 1 = 0x%X\n", epos4_drive_pt[1].ptInParam->StatusWord);
                     printf("Actual Position 0 = %i / %i\n", epos4_drive_pt[0].ptInParam->PositionActualValue - p1,
                            epos4_drive_pt[0].ptOutParam->TargetPosition - p1);
//				printf("Actual Position 1 = %i / %i\n", epos4_drive_pt[1].ptInParam->PositionActualValue-p2, epos4_drive_pt[1].ptOutParam->TargetPosition-p2);
                     osal_usleep(750);

                 }
                 for (i = 1; i <= 5000; i++) {
                     /*ec_send_processdata();
                     wkc = ec_receive_processdata(EC_TIMEOUTRET);

                     if(wkc >= expectedWKC)
                     {
                         printf("Processdata cycle %4d, WKC %d , O:", i, wkc);

                         for(j = 0 ; j < oloop; j++)
                         {
                             printf(" %2.2x", *(ec_slave[0].outputs + j));
                         }

                         printf(" I:");
                         for(j = 0 ; j < iloop; j++)
                         {
                             printf(" %2.2x", *(ec_slave[0].inputs + j));
                         }
                         printf(" T:%"PRId64"\r",ec_DCtime);
                         needlf = TRUE;
                     }*/
                     //epos4_drive_pt[0].ptOutParam->ControlWord = 0x0F;
                     epos4_drive_pt[0].ptOutParam->TargetPosition = 5000 * 50 - i * 50 + p1;
//				epos4_drive_pt[1].ptOutParam->TargetPosition = -i*50+p2;
                     ec_send_processdata();
                     wkc = ec_receive_processdata(EC_TIMEOUTRET);
                     printf("Status word 0 = 0x%X\n", epos4_drive_pt[0].ptInParam->StatusWord);
                     if(epos4_drive_pt[0].ptInParam->StatusWord != 0x1237)break;
//				printf("Status word 1 = 0x%X\n", epos4_drive_pt[1].ptInParam->StatusWord);
                     printf("Actual Position 0 = %i / %i\n", epos4_drive_pt[0].ptInParam->PositionActualValue - p1,
                            epos4_drive_pt[0].ptOutParam->TargetPosition - p1);
//				printf("Actual Position 1 = %i / %i\n", epos4_drive_pt[1].ptInParam->PositionActualValue-p2, epos4_drive_pt[1].ptOutParam->TargetPosition-p2);
                     osal_usleep(750);

                 }
             }

			for (i=0; i<NUMOFEPOS4_DRIVE; ++i)
			   {
				   epos4_drive_pt[i].ptOutParam->ControlWord = 0x00;
					//*(ec_slave[i+1].outputs) = 0x0F000000;
			   }
		       ec_send_processdata();
		       wkc = ec_receive_processdata(EC_TIMEOUTRET);
			   //sleep(1);
			   printf("Status word 0 = 0x%X\n", epos4_drive_pt[0].ptInParam->StatusWord);
//			   printf("Status word 1 = 0x%X\n", epos4_drive_pt[1].ptInParam->StatusWord);
			   osal_usleep(800);

                inOP = FALSE;
            }
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
            //printf("\nRequest init state for all slaves\n");
            //ec_slave[0].state = EC_STATE_INIT;
            /* request INIT state for all slaves */
            //ec_writestate(0);
        }
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
		 printf("Request safe operational state for all slaves\n");
		 ec_slave[0].state = EC_STATE_SAFE_OP;
		 /* request SAFE_OP state for all slaves */
		 ec_writestate(0);
		 /* wait for all slaves to reach state */
		 ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE);
		 ec_slave[0].state = EC_STATE_PRE_OP;
		 /* request SAFE_OP state for all slaves */
		 ec_writestate(0);
		 /* wait for all slaves to reach state */
		 ec_statecheck(0, EC_STATE_PRE_OP,  EC_TIMEOUTSTATE);

	
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

OSAL_THREAD_FUNC ecatcheck( void *ptr )
{
    int slave;
    (void)ptr;                  /* Not used */

    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
               needlf = FALSE;
               printf("\n");
            }
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();
            for (slave = 1; slave <= ec_slavecount; slave++)
            {
               if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
               {
                  ec_group[currentgroup].docheckstate = TRUE;
                  if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                  {
                     printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                     ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                  {
                     printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                     ec_slave[slave].state = EC_STATE_OPERATIONAL;
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state > EC_STATE_NONE)
                  {
                     if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d reconfigured\n",slave);
                     }
                  }
                  else if(!ec_slave[slave].islost)
                  {
                     /* re-check state */
                     ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                     if (ec_slave[slave].state == EC_STATE_NONE)
                     {
                        ec_slave[slave].islost = TRUE;
                        printf("ERROR : slave %d lost\n",slave);
                     }
                  }
               }
               if (ec_slave[slave].islost)
               {
                  if(ec_slave[slave].state == EC_STATE_NONE)
                  {
                     if (ec_recover_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d recovered\n",slave);
                     }
                  }
                  else
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d found\n",slave);
                  }
               }
            }
            if(!ec_group[currentgroup].docheckstate)
               printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);
    }
}

int main(int argc, char *argv[])
{
   printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

   if (argc > 1)
   {
      /* create thread to handle slave error handling in OP */
//      pthread_create( &thread1, NULL, (void *) &ecatcheck, (void*) &ctime);
      osal_thread_create(&thread1, 128000, &ecatcheck, (void*) &ctime);
      /* start cyclic part */
      simpletest(argv[1]);
   }
   else
   {
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");
   }

   printf("End program\n");
   return (0);
}
