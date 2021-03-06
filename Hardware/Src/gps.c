#include "gps.h"
#include "exFlash.h"



/*******************************************************************************
 *
 */
static double GPS_ValueConvert(uint8_t* buf, GPS_LocationTypeEnum loc)
{
	/* 格式：abcde.fghi
	 * 计算方式： abc + (de / 60) + (fghi / 600000) */

	uint8_t  abc  = 0;
	float 	 de   = 0;
	float    fghi = 0;

	switch(loc)
	{
	case GPS_LATITUDE:
		abc  = (*buf * 10) + *(buf + 1);
		de   = (float)((*(buf + 2) * 10) + *(buf + 3)) / 60;
		fghi = (float)((*(buf + 4) * 1000) + (*(buf + 5) * 100) + (*(buf + 6) * 10) + *(buf + 7)) / 600000;
		break;
	case GPS_LONGITUDE:
		abc  = (*buf * 100) + (*(buf + 1) * 10) + *(buf + 2);
		de   = (float)((*(buf + 3) * 10) + *(buf + 4)) / 60;
		fghi = (float)((*(buf + 5) * 1000) + (*(buf + 6) * 100) + (*(buf + 7) * 10) + *(buf + 8)) / 600000;
		break;
	default:
		break;
	}

	return (double)(abc + de + fghi);
}

/*******************************************************************************
 * 获取定位数据
 */
void GPS_GetLocation(uint8_t* buf, GPS_LocateTypedef* info)
{
	uint8_t latitude[8]  = {0};
	uint8_t longitude[9] = {0};

	/* 判断数据有效 */
	if (GPS_FLAG_VALID == *(buf + GPS_OFFSET_FLAG))
	{
		str2numb((buf + GPS_OFFSET_LATITUDE),     &latitude[0], 4);
		str2numb((buf + GPS_OFFSET_LATITUDE + 5), &latitude[4], 4);

		str2numb(buf + GPS_OFFSET_LONGITUDE,     &longitude[0], 5);
		str2numb(buf + GPS_OFFSET_LONGITUDE + 6, &longitude[5], 4);

		/* 经纬度获取 */
		info->longitude = GPS_ValueConvert(longitude, GPS_LONGITUDE);
		info->latitude  = GPS_ValueConvert(latitude,  GPS_LATITUDE);

		if (*(buf + GPS_OFFSET_LATITUDE_FLAG) == 'S')
			info->latitude = -(info->latitude);
		if (*(buf + GPS_OFFSET_LATITUDE_FLAG) == 'W')
			info->longitude = -(info->longitude);
	}
}


