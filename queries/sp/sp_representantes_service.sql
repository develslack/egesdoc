DELIMITER //


CREATE PROCEDURE sp_insertar_representante(
    IN p_nombre_representante VARCHAR(100),
    IN d_dni_representante VARCHAR(10),
    IN p_email_representante varchar(50)
)
BEGIN
    INSERT INTO representantes (nombre_representante, dni_representante, email_representante)
    VALUES (p_nombre_representante, p_dni_representante, p_email_representante);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_representante(
    IN p_id INT,
    IN p_nombre_representante VARCHAR(100),
    IN d_dni_representante VARCHAR(10),
    IN p_email_representante varchar(50)
)
BEGIN
    UPDATE representantes
    SET nombre_representante = p_nombre_representante,
        dni_representante = p_dni_representante,
        email_representante = p_email_representante
    WHERE id = p_id;
END //

DELIMITER ;
