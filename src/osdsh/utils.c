#include "osdsh.h"
#include "../config.h"

/*This function is used to control the options of the individual osd's*/

void control_options(settings_t *setts, char option[2], char argument[BUFSIZ])
{
    /*this function reads the option to change on each display, changes it
    and saves the changes on the corresponding settings structure*/

    switch (option[0]) {
	case 'f':
	    strncpy(setts->font, argument, BUFSIZ);
	    if (setts->displaying&&xosd_set_font(setts->myosd, argument)) 
		fprintf(stderr, "Couldn't change font\n"); 
    	break;
    	case 'C':
	    strncpy(setts->color, argument, BUFSIZ);
	    if (setts->displaying&&xosd_set_colour(setts->myosd, argument))
		fprintf(stderr, "Couldn't change color\n");
	break;
	case 'd':
	    setts->delay = atoi(argument);
	    if (setts->displaying&&xosd_set_timeout(setts->myosd, atoi(argument)))
		fprintf(stderr, "Couldn't change timeout\n");
	break;
	case 'b':
	    setts->position = XOSD_bottom;
	    if (setts->displaying&&xosd_set_pos(setts->myosd, XOSD_bottom))
		fprintf(stderr, "Couldn't change position\n");
	break;
	case 't':
	    setts->position = XOSD_top;
	    if (setts->displaying&&xosd_set_pos(setts->myosd, XOSD_top))
		fprintf(stderr, "Couldn't change position\n");
	break;
	case 'l':
	    setts->align = XOSD_left;
	    if (setts->displaying&&xosd_set_align(setts->myosd, XOSD_left))
		fprintf(stderr, "Couldn't change align\n");
	break;
	case 'r':
	    setts->align = XOSD_right;
	    if (setts->displaying&&xosd_set_align(setts->myosd, XOSD_right))
		fprintf(stderr, "Couldn't change align\n");
	break;
	case 'c':
	    setts->align = XOSD_center;
	    if (setts->displaying&&xosd_set_align(setts->myosd, XOSD_center))
		fprintf(stderr, "Couldn't change align\n");
	case 'o':
	    setts->soffset = atoi(argument);
	    if (setts->displaying&&xosd_set_shadow_offset(setts->myosd, setts->soffset))
		fprintf(stderr, "Couldn't change Shadow Offset\n");
	break;
	case 'y':
	    setts->yoffset = atoi(argument);
	    if (setts->displaying&&xosd_set_vertical_offset(setts->myosd, setts->yoffset))
		fprintf(stderr, "Couldn't change vertical offset\n");
	    break;
	case 'x':
	    setts->xoffset = atoi(argument);
	    if (setts->displaying&&xosd_set_horizontal_offset(setts->myosd, setts->xoffset))
		fprintf(stderr, "Couldn't change horizontal offset\n");
	default:
	break;
    }
}

/*============================= initialize_osd() =========================== */
void initialize_osd( settings_t *setts)
{
	xosd_set_font(setts->myosd, setts->font);
	xosd_set_colour(setts->myosd, setts->color);
	xosd_set_pos(setts->myosd, setts->position);
	xosd_set_align(setts->myosd, setts->align);
	xosd_set_horizontal_offset(setts->myosd, setts->xoffset);
	xosd_set_vertical_offset(setts->myosd, setts->yoffset);
	xosd_set_shadow_offset(setts->myosd, setts->soffset);
	xosd_set_timeout(setts->myosd, setts->delay);
}
/* ============================ set_defaults () ============================ */ 
void set_defaults(settings_t *setts)
{
    strncpy(setts->font, FONT, BUFSIZ);
    setts->font[BUFSIZ] = '\0';
    strncpy(setts->color, COLOR, BUFSIZ);
    setts->color[BUFSIZ] = '\0';
    setts->soffset = SHADOW_OFFSET;
    setts->yoffset = DEFAULT_OFFSET;
    setts->xoffset = DEFAULT_OFFSET;
    setts->align= OSD_ALIGNMENT;
    setts->position = OSD_POSITION;
    setts->delay = TIMEOUT;
    setts->displaying = 0;
}

