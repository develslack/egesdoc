DELIMITER //


CREATE PROCEDURE sp_insertar_adicional_grado(
    IN p_nivel VARCHAR(1),
    IN p_grado VARCHAR(2),
    IN p_cant_ur INT

)
BEGIN
    INSERT INTO adicional_grado_ur (nivel, grado, cant_ur)
    VALUES (p_nivel, p_grado, p_cant_ur);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_adicional_grado(
    IN p_id INT,
    IN p_nivel VARCHAR(1),
    IN p_grado VARCHAR(2),
    IN p_cant_ur INT
)
BEGIN
    UPDATE adicional_grado_ur
    SET nivel = p_nivel, grado = p_grado, cant_ur = p_cant_ur
    WHERE id = p_id;
END //

DELIMITER ;
