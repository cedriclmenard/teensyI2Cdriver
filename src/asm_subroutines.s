
.extern I2C0_D

.section .text, "x"

.global dummy_read_I2C_data

dummy_read_I2C_data:
    LDR r0, =I2C0_D   @ Load data register address
    LDR r0, [r0]      @ Load data from register
    BX lr             @ Return
