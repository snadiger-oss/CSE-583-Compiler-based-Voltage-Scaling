#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void) {
    char lorem[] = "Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.";
    char ipsum[sizeof(lorem)];
    for (int i = 0; i < sizeof(lorem); ++i)
    {
        ipsum[i] = lorem[i];
    }
    assert(!strcmp(lorem, ipsum));
    puts(ipsum);
}