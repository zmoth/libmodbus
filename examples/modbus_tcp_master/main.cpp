#include <iostream>

#include <modbus.h>

int main()
{
    modbus_t *ctx;
    uint16_t tab_reg[64];
    int rc;

    ctx = modbus_new_tcp("127.0.0.1", 502);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return EXIT_FAILURE;
    }

    if (modbus_set_slave(ctx, 1) == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        modbus_close(ctx);
        modbus_free(ctx);
        return EXIT_FAILURE;
    }

    while (true) {
        Sleep(2000);

        rc = modbus_read_registers(ctx, 1036, 18, tab_reg);
        if (rc == -1) {
            fprintf(stderr, "%s\n", modbus_strerror(errno));
            continue;
        }

        const float x = modbus_get_float_abcd(&tab_reg[0]);
        const int x_is_valid = tab_reg[2];
        const float y = modbus_get_float_abcd(&tab_reg[3]);
        const int y_is_valid = tab_reg[5];
        const float z = modbus_get_float_abcd(&tab_reg[6]);
        const int z_is_valid = tab_reg[8];
        const float rx = modbus_get_float_abcd(&tab_reg[9]);
        const int rx_is_valid = tab_reg[11];
        const float ry = modbus_get_float_abcd(&tab_reg[12]);
        const int ry_is_valid = tab_reg[14];
        const float rz = modbus_get_float_abcd(&tab_reg[15]);
        const int rz_is_valid = tab_reg[17];

        printf("[%d]x: %f;", x_is_valid, x);
        printf("[%d]y: %f;", y_is_valid, y);
        printf("[%d]z: %f;", z_is_valid, z);
        printf("[%d]rx: %f;", rx_is_valid, rx);
        printf("[%d]ry: %f;", ry_is_valid, ry);
        printf("[%d]rz: %f\n", rz_is_valid, rz);
    }

    modbus_close(ctx);
    modbus_free(ctx);

    return EXIT_SUCCESS;
}
