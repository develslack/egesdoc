console.clear();
console.log("✅ Módulo Unidades Retributivas Iniciado de forma nativa.");

// CORRECCIÓN: Usamos el objeto window para evitar el SyntaxError de redeteclaración con let
window.dTable = window.dTable || null;

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL +"/unidades_retributivas/list");
    const registros = await response.json();

    const tableBody = document.getElementById("unidadesRetributivasTableBody");
    const alertInfo = document.getElementById("alert-info-unidades_retributivas");
    tableBody.innerHTML = "";

    let count = 0;

    registros.forEach((reg) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td class="text-center">${reg.nivel}</td>
        <td class="text-center">${reg.grado}</td>
        <td class="text-center">${reg.sueldo_ur}</td>
        <td class="text-center">${reg.dedicacion_funcional_ur}</td>
        <td class="text-center">${reg.total_ur}</td>
        <td class="text-center">
          <button class="btn btn-default btn-sm btn-editar-unidad_retributiva"
                  data-id="${reg.id}"
                  data-nivel="${reg.nivel}"
                  data-grado="${reg.grado}"
                  data-sueldo_ur="${reg.sueldo_ur}"
                  data-dedicacion_funcional_ur="${reg.dedicacion_funcional_ur}"
                  data-total_ur="${reg.total_ur}">
            <span class="glyphicon glyphicon-edit"></span> Editar
          </button>
        </td>`;

      tableBody.appendChild(row);
      count++;
    });

    if (alertInfo) {
        alertInfo.innerHTML = `<div class="alert alert-info">
                                <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                               </div><hr>`;
    }

    if ($.fn.DataTable.isDataTable("#unidadesRetributivasTable")) {
      $("#unidadesRetributivasTable").DataTable().destroy();
    }

    dTable = $('#unidadesRetributivasTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender":    true,
        "retrieve":       true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            { extend: 'excel', text: '<i class="glyphicon glyphicon-list-alt"></i> Export Excel', exportOptions: { columns: ':visible'} },
            { extend: 'csv', text: '<i class="glyphicon glyphicon-align-justify"></i> Export CSV', exportOptions: { columns: ':visible'} },
            { extend: 'pdf', text: '<i class="glyphicon glyphicon-file"></i> Export PDF', exportOptions: { columns: ':visible'} },
            {
                extend: 'print',
                text: '<i class="glyphicon glyphicon-print"></i> Imprimir',
                customize: function ( win ) {
                    $(win.document.body).css( 'font-size', '8pt' );$(win.document.body).find( 'table' ).addClass( 'compact' ).css( 'font-size', 'inherit' );
                },
                autoPrint: false,
                exportOptions: { columns: ':visible' }
            },
            'colvis'
        ],
        "language": {
            "lengthMenu": "Mostrar _MENU_ registros por pagina",
            "info": "Mostrando pagina _PAGE_ de _PAGES_",
            "search": "Buscar:",
            "zeroRecords":    "No se encontraron registros coincidentes",
            "paginate": { "next": "Siguiente", "previous": "Anterior" }
        }
    });

    // ================================================================================================================= //
    // 2. PREPARACIÓN DEL MODAL USANDO EL SISTEMA DEL DASHBOARD
    // ================================================================================================================= //
    // Usamos el fetch nativo del servidor para rellenar el div de abajo sin pisar el dashboard central
    const contenedorModal = document.getElementById("contenedor-modal-maestro-unidades_retributivas");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/unidades_retributivas/nueva_unidad_retributiva.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    // ================================================================================================================= //
    // 3. CAPTURA SEGURO DE EVENTOS (DELEGACIÓN DE EVENTOS JQUERY)
    // ================================================================================================================= //

    // Evento Añadir (Alta)
    $(document).off("click", "#add-unidad_retributiva-form").on("click", "#add-unidad_retributiva-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    // Evento Editar (Fila de la tabla)
    $(document).off("click", ".btn-editar-unidad_retributiva").on("click", ".btn-editar-unidad_retributiva", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nivel: $(this).attr("data-nivel"),
            grado: $(this).attr("data-grado"),
            sueldo_ur: $(this).attr("data-sueldo_ur"),
            dedicacion_funcional_ur: $(this).attr("data-dedicacion_funcional_ur"),
            total_ur: $(this).attr("data-total_ur")
        };
        prepararYMostrarModal("edicion", datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Unidades Retributivas:", error);
  }
})();

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUYE EL FORMULARIO INTERNO Y MUESTRA EL MODAL FLOTANTE
// ===================================================================================================================== //
function prepararYMostrarModal(modo, datos = null) {
    const $modal =$("#myModal-unidad_retributiva");
    if ($modal.length === 0) return;

    // Seteamos título del modal flotante
    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Unidad Retributiva" : "Editar Unidad Reributiva");

    // Seteamos campos en el body vacío
    $modal.find(".modal-body").html(`

        <form id="form-modal-unidad_retributiva">
            <input type="hidden" id="modal-unidad_retributiva-id" value="${datos ? datos.id : ''}">
            <div class="form-group">
                <label>Nivel:</label>
                <input type="text" class="form-control" id="modal-unidad_retributiva-nivel" value="${datos ? datos.nivel : ''}" required>
            </div>
            <div class="form-group">
                <label>Grado:</label>
                <input type="text" class="form-control" id="modal-unidad_retributiva-grado" value="${datos ? datos.grado : ''}" required>
            </div>
            <div class="form-group">
                <label>Sueldo UR:</label>
                <input type="text" class="form-control" id="modal-unidad_retributiva-sueldo_ur" value="${datos ? datos.sueldo_ur : ''}" required>
            </div>
            <div class="form-group">
                <label>Dedicación Funcional UR:</label>
                <input type="text" class="form-control" id="modal-unidad_retributiva-dedicacion_funcional_ur" value="${datos ? datos.dedicacion_funcional_ur : ''}" required>
            </div>
            <div class="form-group">
                <label>Total UR:</label>
                <input type="text" class="form-control" id="modal-unidad_retributiva-total_ur" value="${datos ? datos.total_ur : ''}" required readonly>
            </div>
            <button type="submit" id="btn-submit-oculto" style="display:none;"></button>
        </form>
    `);

    // Seteamos barra de estado y botones en el footer vacío
    $modal.find(".modal-footer").html(`
        <div id="modal-status-message" style="margin-bottom: 10px; text-align: left;"></div>
        <div class="text-right">
            <button type="button" class="btn btn-danger" data-dismiss="modal">
                <span class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span> Cerrar</button>
            <button type="button" class="btn btn-success" id="btn-modal-guardar">
                <span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Guardar</button>
        </div>
    `);

    // Hacemos que aparezca de forma flotante sobre dashboard.html
    $modal.modal("show");

    // =========================================================================
    // NUEVO CÓDIGO: CÁLCULO AUTOMÁTICO EL TOTAL DE UR
    // =========================================================================
    const inputSueldoUr = document.getElementById("modal-unidad_retributiva-sueldo_ur");
    const inputDedicacionUr = document.getElementById("modal-unidad_retributiva-dedicacion_funcional_ur");
    const inputTotal = document.getElementById("modal-unidad_retributiva-total_ur");

    inputTotal.addEventListener("focus", function() {
        const sueldo = parseInt(inputSueldoUr.value) || 0;
        const dedicacion = parseInt(inputDedicacionUr.value) || 0;
        const resultado = (sueldo + dedicacion);
        this.value = resultado;
    });
    // =========================================================================

    // =========================================================================

    // Vinculamos el botón verde del footer con la validación del form
    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    // Procesamiento del submit al backend en C
    document.getElementById("form-modal-unidad_retributiva").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        const id = document.getElementById("modal-unidad_retributiva-id").value;

        const payload = {
            nivel: document.getElementById("modal-unidad_retributiva-nivel").value,
            grado: document.getElementById("modal-unidad_retributiva-grado").value,
            sueldo_ur: document.getElementById("modal-unidad_retributiva-sueldo_ur").value,
            dedicacion_funcional_ur: document.getElementById("modal-unidad_retributiva-dedicacion_funcional_ur").value,
            total_ur: document.getElementById("modal-unidad_retributiva-total_ur").value
        };

        console.log(id);

        if(id == ""){

            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/unidades_retributivas/add", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `nivel=${encodeURIComponent(payload.nivel)}&grado=${encodeURIComponent(payload.grado)}&sueldo_ur=${encodeURIComponent(payload.sueldo_ur)}&dedicacion_funcional_ur=${encodeURIComponent(payload.dedicacion_funcional_ur)}&total_ur=${encodeURIComponent(payload.total_ur)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/unidades_retributivas/unidades_retributivas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Ha ocurrido un problema al procesar la solicitud.</span>`;
            }

        }

        console.log(id);

        if(id != ""){

            console.log(payload);
            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/unidades_retributivas/edit", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `id=${encodeURIComponent(id)}&nivel=${encodeURIComponent(payload.nivel)}&grado=${encodeURIComponent(payload.grado)}&sueldo_ur=${encodeURIComponent(payload.sueldo_ur)}&dedicacion_funcional_ur=${encodeURIComponent(payload.dedicacion_funcional_ur)}&total_ur=${encodeURIComponent(payload.total_ur)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/unidades_retributivas/unidades_retributivas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Ha ocurrido un problema al procesar la solicitud.</span>`;
            }

        }

    };
}
