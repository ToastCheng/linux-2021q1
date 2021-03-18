uint16_t func(uint16_t N) {
    /* change all right side bits to 1 */
    N |= N >> 1;
    N |= N >> /* X */ 2;
    N |= N >> /* Y */ 4;
    N |= N >> /* Z */ 8;

    return (N + 1) >> 1;
}