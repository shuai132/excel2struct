#pragma once

/*
Block           ${BLOCK}
Address         ${OFFSET}
Register Name   ${NAME}
R/W             ${RW} [__I OR __O OR __IO ]
Width           ${WIDTH}

#define  _TV303_${BLOCK}_${OFFSET}      (_TV303_V_NR_BASE + 0x${OFFSET})

typedef struct
{
    ${RW} uint32_t ${NAME}              : ${WIDTH};
} ${BLOCK}_${OFFSET}_TypeDef;

typedef union {
    uint32_t                            all;
    ${BLOCK}_${OFFSET}_TypeDef          bit;
} ${BLOCK}_${OFFSET}_REG;

#define ${BLOCK}_${OFFSET} ((${BLOCK}_${OFFSET}_REG *)((uint32_t)SVP_REG_MIPS(_TV303_${BLOCK}_${OFFSET})))
 */

const char* TEMPLATE_DEFINE = R"(
#define  _TV303_${BLOCK}_${OFFSET}      (_TV303_V_NR_BASE + 0x${OFFSET})

typedef struct
{
${STRUCT_PROP}
} ${BLOCK}_${OFFSET}_TypeDef;

typedef union {
    uint32_t                            all;
    ${BLOCK}_${OFFSET}_TypeDef          bit;
} ${BLOCK}_${OFFSET}_REG;

#define ${BLOCK}_${OFFSET} ((${BLOCK}_${OFFSET}_REG *)((uint32_t)SVP_REG_MIPS(_TV303_${BLOCK}_${OFFSET})))
)";

const char* TEMPLATE_PROP = R"(${RW} uint32_t ${NAME} : ${WIDTH}; )";
