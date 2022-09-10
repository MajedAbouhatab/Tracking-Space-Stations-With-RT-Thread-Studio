#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "lvgl.h"
#include <world_06_en.h>
#include <stdlib.h>

#define SendToESP(U) rt_device_write(serial, 0, U, (sizeof(U) - 1)); rt_thread_mdelay(3000)

lv_obj_t *SpacePeople, *ISS, *Tiangong;
int SSWidth = 40;

static struct rt_semaphore rx_sem;
static rt_device_t serial;

static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
    char SingleChar, Buffer[RT_SERIAL_RB_BUFSZ];
    int pos = 0;
    while (1)
    {
        while (rt_device_read(serial, -1, &SingleChar, 1) != 1)
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        Buffer[pos++] = '\0';
        rt_sprintf(Buffer, "%s%c", Buffer, SingleChar);
        if (SingleChar == '\n')
        {
            char *TextPiece, *TextRemaining, *SpaceStation;
            TextRemaining = Buffer;
            while ((TextPiece = strtok_r(TextRemaining, "\"", &TextRemaining)))
            {
                if (strlen(TextPiece) > 5)
                {
                    if (!strcmp(TextPiece, "number"))
                    {
                        rt_sprintf(Buffer, "");
                        while ((TextPiece = strtok_r(TextRemaining, "\"", &TextRemaining)))
                            if (!strcmp(TextPiece, "name"))
                            {
                                for (int i = 0; i < 2; i++)
                                    TextPiece = strtok_r(TextRemaining, "\"", &TextRemaining);
                                rt_sprintf(Buffer, "%s   %s", Buffer, TextPiece);
                                for (int i = 0; i < 4; i++)
                                    TextPiece = strtok_r(TextRemaining, "\"", &TextRemaining);
                                rt_sprintf(Buffer, "%s (%s)", Buffer, TextPiece);
                            }
                        if (strcmp(lv_label_get_text(SpacePeople), Buffer))
                            lv_label_set_text(SpacePeople, Buffer);
                        break;
                    }
                    else
                    {
                        TextRemaining = TextPiece;
                        int Lat = atoi(strtok_r(TextRemaining, "|", &TextRemaining)), Lon = atoi(
                                strtok_r(TextRemaining, "|", &TextRemaining));
                        lv_obj_set_pos(!strcmp(SpaceStation, "ISS") ? ISS : Tiangong,
                                ((Lon + 180.0) / 360.0) * world_06_en.header.w - SSWidth / 2,
                                ((-1.0 * Lat + 90.0) / 180.0) * world_06_en.header.h - SSWidth / 2);
                        break;
                    }
                }
                else
                {
                    if (strcmp(TextPiece, "25544") == 0)
                        SpaceStation = "ISS";
                    else if (strcmp(TextPiece, "48274") == 0)
                        SpaceStation = "Tiangong";
                }
            }
            pos = 0;
            rt_sprintf(Buffer, "");
        }
    }
}

void lv_user_gui_init(void)
{
    static lv_style_t MainBackground;

    lv_style_init(&MainBackground);
    lv_style_set_bg_color(&MainBackground, lv_color_black());
    lv_obj_add_style(lv_scr_act(), &MainBackground, 0);

    lv_img_set_src(lv_img_create(lv_scr_act()), &world_06_en);

    ISS = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(ISS, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_radius(ISS, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(ISS, SSWidth, SSWidth);
    lv_obj_set_pos(ISS, -1 * SSWidth, -1 * SSWidth);

    Tiangong = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(Tiangong, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_radius(Tiangong, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(Tiangong, SSWidth, SSWidth);
    lv_obj_set_pos(Tiangong, -1 * SSWidth, -1 * SSWidth);

    SpacePeople = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(SpacePeople, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(SpacePeople, 800);
    lv_label_set_text(SpacePeople, "    open-notify.org");
    lv_obj_align(SpacePeople, LV_ALIGN_BOTTOM_RIGHT, 0, -25);
    lv_obj_set_style_text_color(SpacePeople, lv_color_white(), 0);

    lv_obj_t *SpaceStationKeys = lv_label_create(lv_scr_act());
    lv_obj_set_width(SpaceStationKeys, 800);
    lv_label_set_text(SpaceStationKeys, "ISS\t\t\t\tTiangong");
    lv_obj_align(SpaceStationKeys, LV_ALIGN_BOTTOM_LEFT, 50, -85);

    lv_obj_t *ISSKey = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(ISSKey, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_set_style_radius(ISSKey, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(ISSKey, SSWidth * 0.75, SSWidth * 0.75);
    lv_obj_align(ISSKey, LV_ALIGN_BOTTOM_LEFT, 10, -90);

    lv_obj_t *TiangongKey = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(TiangongKey, lv_palette_main(LV_PALETTE_PURPLE), 0);
    lv_obj_set_style_radius(TiangongKey, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(TiangongKey, SSWidth * 0.75, SSWidth * 0.75);
    lv_obj_align(TiangongKey, LV_ALIGN_BOTTOM_LEFT, 150, -90);
}

int main(void)
{
    serial = rt_device_find("uart6");
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(serial, uart_input);
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 2 * RT_SERIAL_RB_BUFSZ, 25, 10);
    rt_thread_startup(thread);
    while (1)
    {
        SendToESP("http://api.open-notify.org/astros.json");
        SendToESP("https://www.n2yo.com/sat/instant-tracking.php?s=25544&d=1");
        SendToESP("https://www.n2yo.com/sat/instant-tracking.php?s=48274&d=1");
    }
}
