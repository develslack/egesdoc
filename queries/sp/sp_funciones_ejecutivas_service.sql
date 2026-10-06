DELIMITER //


CREATE PROCEDURE sp_insertar_funcion_ejecutiva(
    IN p_nivel VARCHAR(1),
    IN p_cant_ur INT,
    IN p_valor_ur FLOAT(8,2),
    IN p_monto FLOAT(8,2),
    IN p_norma_regulatoria VARCHAR(100),
    IN p_f_entrada_vigencia DATE,
    IN p_mes VARCHAR(10),
    IN p_anio VARCHAR(4)
)
BEGIN
    INSERT INTO funciones_ejecutivas (nivel, cant_ur, valor_ur, monto, norma_regulatoria, f_entrada_vigencia, mes, anio)
    VALUES (p_nivel, p_cant_ur, p_valor_ur, p_monto, p_norma_regulatoria, p_f_entrada_vigencia, p_mes, p_anio);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_funcion_ejecutiva(
    IN p_id INT,
    IN p_nivel VARCHAR(1),
    IN p_cant_ur INT,
    IN p_valor_ur FLOAT(8,2),
    IN p_monto FLOAT(8,2),
    IN p_norma_regulatoria VARCHAR(100),
    IN p_f_entrada_vigencia DATE,
    IN p_mes VARCHAR(10),
    IN p_anio VARCHAR(4)
)
BEGIN
    UPDATE funciones_ejecutivas
    SET nivel = p_nivel, cant_ur = p_cant_ur, valor_ur = p_valor_ur, monto = p_monto, norma_regulatoria = p_norma_regulatoria, f_entrada_vigencia = p_f_entrada_vigencia, mes = p_mes, anio = p_anio
    WHERE id = p_id;
END //

DELIMITER ;
