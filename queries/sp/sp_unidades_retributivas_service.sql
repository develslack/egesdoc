DELIMITER //


CREATE PROCEDURE sp_insertar_unidades_retributivas(
    IN p_nivel VARCHAR(1),
    IN p_grado VARCHAR(2),
    IN p_sueldo_ur INT,
    IN p_dedicacion_funcional_ur INT,
    IN p_total_ur INT

)
BEGIN
    INSERT INTO unidades_retributivas (nivel, grado, sueldo_ur, dedicacion_funcional_ur, total_ur)
    VALUES (p_nivel, p_grado,p_sueldo_ur, p_dedicacion_funcional_ur, p_total_ur);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_unidades_retributivas(
    IN p_id INT,
    IN p_nivel VARCHAR(1),
    IN p_grado VARCHAR(2),
    IN p_sueldo_ur INT,
    IN p_dedicacion_funcioal_ur INT,
    IN p_total_ur INT
)
BEGIN
    UPDATE unidades_retributivas
    SET nivel = p_nivel, grado = p_grado, sueldo_ur = p_sueldo_ur, dedicacion_funcional_ur = p_dedicacion_funcional_ur, total_ur = p_total_ur
    WHERE id = p_id;
END //

DELIMITER ;
