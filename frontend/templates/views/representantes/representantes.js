console.clear();
console.log("✅ Módulo Representantes Iniciado de forma nativa.");

// CORRECCIÓN: Usamos el objeto window para evitar el SyntaxError de redeteclaración con let
window.dTable = window.dTable || null;

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL +"/representantes/list");
    const registros = await response.json();

    const tableBody = document.getElementById("representantesTableBody");
    const alertInfo = document.getElementById("alert-info-representantes");
    tableBody.innerHTML = "";

    let count = 0;

    registros.forEach((reg) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td class="text-center">${reg.nombre_representante}</td>
        <td class="text-center">${reg.dni_representante}</td>
        <td class="text-center">${reg.email_representante}</td>
        <td class="text-center">
          <button class="btn btn-default btn-sm btn-editar-representantes"
                  data-id="${reg.id}"
                  data-nombre_representante="${reg.nombre_representante}"
                  data-dni_representante="${reg.dni_representante}"
                  data-email_representante="${reg.email_representante}">
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

    if ($.fn.DataTable.isDataTable("#representantesTable")) {
      $("#representantesTable").DataTable().destroy();
    }

    dTable = $('#representantesTable').DataTable({
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
    const contenedorModal = document.getElementById("contenedor-modal-maestro-representantes");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/representantes/nuevo_representante.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    // ================================================================================================================= //
    // 3. CAPTURA SEGURO DE EVENTOS (DELEGACIÓN DE EVENTOS JQUERY)
    // ================================================================================================================= //

    // Evento Añadir (Alta)
    $(document).off("click", "#add-representantes-form").on("click", "#add-representantes-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    // Evento Editar (Fila de la tabla)
    $(document).off("click", ".btn-editar-representantes").on("click", ".btn-editar-representantes", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre_representante: $(this).attr("data-nombre_representante"),
            dni_representante: $(this).attr("data-dni_representante"),
            email_representante: $(this).attr("data-email_representante")
        };
        prepararYMostrarModal("edicion", datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Representantes:", error);
  }
})();

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUYE EL FORMULARIO INTERNO Y MUESTRA EL MODAL FLOTANTE
// ===================================================================================================================== //
function prepararYMostrarModal(modo, datos = null) {
    const $modal =$("#myModal-representantes");
    if ($modal.length === 0) return;

    // Seteamos título del modal flotante
    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Representante" : "Editar Representante");

    // Seteamos campos en el body vacío
    $modal.find(".modal-body").html(`

        <form id="form-modal-representantes">
            <input type="hidden" id="modal-representantes-id" value="${datos ? datos.id : ''}">
            <div class="form-group">
                <label><span class="label label-default">Nombre y Apellido</span></label>
                <input type="text" class="form-control" id="modal-representantes-nombre_representante" value="${datos ? datos.nombre_representante : ''}" required>
            </div>
            <div class="form-group">
                <label><span class="label label-default">DNI Representante</span></label>
                <input type="text" class="form-control" id="modal-representantes-dni_representante" value="${datos ? datos.dni_representante : ''}" maxlength="10" required>
            </div>
            <div class="form-group">
                <label><span class="label label-default">Email Representante</span></label>
                <input type="email" class="form-control" id="modal-representantes-email_representante" value="${datos ? datos.email_representante : ''}" required>
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

    // Vinculamos el botón verde del footer con la validación del form
    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    // Procesamiento del submit al backend en C
    document.getElementById("form-modal-representantes").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        const id = document.getElementById("modal-representantes-id").value;

        const payload = {
            nombre_representante: document.getElementById("modal-representantes-nombre_representante").value,
            dni_representante: document.getElementById("modal-representantes-dni_representante").value,
            email_representante: document.getElementById("modal-representantes-email_representante").value
        };

        console.log(id);

        if(id == ""){

            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/representantes/add", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `nombre_representante=${encodeURIComponent(payload.nombre_representante.toUpperCase())}&dni_representante=${encodeURIComponent(payload.dni_representante)}&email_representante=${encodeURIComponent(payload.email_representante)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/representantes/representantes.html");
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
                const enviorresp = await fetch(window.API_BASE_URL +"/representantes/edit", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `id=${encodeURIComponent(id)}&nombre_representante=${encodeURIComponent(payload.nombre_representante.toUpperCase())}&dni_representante=${encodeURIComponent(payload.dni_representante)}&email_representante=${encodeURIComponent(payload.email_representante)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/representantes/representantes.html");
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
