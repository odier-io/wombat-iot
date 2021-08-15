/* Author : Jerome ODIER
 * Email : jerome@odier.xyz
 *
 * Version : 1.0 (2016-![VALUE YEAR])
 *
 *
 * This file is part of WOMBAT-IOT.
 *
 *
 * Batkens RESTRICTED
 * __________________
 *
 *  Batkens
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Batkens and its suppliers, if any.
 * The intellectual and technical concepts contained herein are
 * proprietary to Batkens and its suppliers and are protected by
 * trade secret or copyright law. Dissemination of this information
 * or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained from Batkens.
 */

/*--------------------------------------------------------------------------------------------------------------------*/

#ifndef __IOT_DEFAULT_H_
#define __IOT_DEFAULT_H_

/*--------------------------------------------------------------------------------------------------------------------*/

#define IOT_DEFAULT_IOT_UID							__EXPAND(IOT_UID)
#define IOT_DEFAULT_IOT_DESCR						__EXPAND(IOT_DESCR)
#define IOT_DEFAULT_IOT_SLEEP_MS					1000

/*--------------------------------------------------------------------------------------------------------------------*/

#define IOT_DEFAULT_MQTT_SERVER_URL					"tcp://localhost:1883"
#define IOT_DEFAULT_MQTT_SERVER_USER				NULL
#define IOT_DEFAULT_MQTT_SERVER_PASS				NULL
#define IOT_DEFAULT_MQTT_CONNECT_TIMEOUT_MS			10000
#define IOT_DEFAULT_MQTT_DISCONNECT_TIMEOUT_MS		1000
#define IOT_DEFAULT_MQTT_LOG_LEVEL					7

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* __IOT_DEFAULT_H_ */

/*--------------------------------------------------------------------------------------------------------------------*/
